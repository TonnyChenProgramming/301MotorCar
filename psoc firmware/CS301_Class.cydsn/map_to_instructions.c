#include "map.h"
#include "map_to_instructions.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define ROWS 15
#define COLS 19

#ifndef USE_USB
#define USE_USB 1
#endif

#if USE_USB
extern void usbPutString(char *s);
static void dbg_puts(const char *s) {}
#else
static void dbg_puts(const char *s) {}
#endif

// Directions: 0 = right, 1 = down, 2 = left, 3 = up
static const int drow[4] = {0, 1, 0, -1};
static const int dcol[4] = {1, 0, -1, 0};

static inline bool in_bounds(int r, int c) {
    return (r >= 0 && r < ROWS && c >= 0 && c < COLS);
}
static inline bool is_free(int r, int c) {
    return in_bounds(r, c) && (map[r][c] == 0);
}
static inline int heuristic(int r, int c, int gr, int gc) {
    return abs(r - gr) + abs(c - gc);
}

typedef struct {
    int r, c, f;
} Node;

static inline bool has_side_branch(int r, int c, int came_dir, int go_dir) {
    for (int d = 0; d < 4; ++d) {
        if (!is_free(r + drow[d], c + dcol[d])) continue;
        if (d != came_dir && d != go_dir) return true;
    }
    return false;
}



static void emit_plan_for_path(const int path[][2], int len,
                               RobotInstr instr[], int *count, int max_instr,
                               int *heading_ptr,
                               int *food_dists_ptr, int *food_axes_ptr, int current_food_index)

{
    if (len < 2) return;
    int idx = *count;
    int heading = *heading_ptr;

    if (heading == -1) {
        int r0 = path[0][0], c0 = path[0][1];
        int r1 = path[1][0], c1 = path[1][1];
        for (int d = 0; d < 4; ++d)
            if (r0 + drow[d] == r1 && c0 + dcol[d] == c1)
                heading = d;
    }

    int straight_decisions = 0;

    for (int i = 1; i < len && idx < max_instr; ++i) {
        int r_prev = path[i - 1][0], c_prev = path[i - 1][1];
        int r_here = path[i][0],      c_here = path[i][1];

        int step_dir = -1;
        for (int d = 0; d < 4; ++d)
            if (r_prev + drow[d] == r_here && c_prev + dcol[d] == c_here)
                step_dir = d;

        int came_dir = (step_dir + 2) & 3;
        int go_dir   = step_dir;

        bool is_goal = (i == len - 1);
        bool decision_point = is_goal || has_side_branch(r_here, c_here, came_dir, go_dir);

        int diff = (step_dir - heading + 4) & 3;

        if (diff != 0) {
            for (int k = 0; k < straight_decisions && idx < max_instr; ++k)
                instr[idx++].type = iSTRAIGHT;
            straight_decisions = 0;

            if (diff == 1 && idx < max_instr)      instr[idx++].type = iRIGHT;
            else if (diff == 3 && idx < max_instr) instr[idx++].type = iLEFT;
            else if (diff == 2 && idx < max_instr) instr[idx++].type = iTURN_AROUND;

            heading = step_dir;
        } 
        else if (decision_point) {
            straight_decisions++;
        }

        if (idx >= max_instr - 3)
            break;
    }

    for (int k = 0; k < straight_decisions && idx < max_instr; ++k)
        instr[idx++].type = iSTRAIGHT;

    if (straight_decisions == 0 && len > 1 && idx < max_instr) {
        int r_last = path[len-2][0], c_last = path[len-2][1];
        int r_goal = path[len-1][0], c_goal = path[len-1][1];
        if (abs(r_last - r_goal) + abs(c_last - c_goal) == 1)
            instr[idx++].type = iSTRAIGHT;
    }

    if (food_dists_ptr) {
        int straight_to_goal = 0;
        int goal_r = path[len - 1][0], goal_c = path[len - 1][1];
        int prev_r = path[len - 2][0], prev_c = path[len - 2][1];

        int final_dir = -1;
        for (int d = 0; d < 4; ++d)
            if (goal_r + drow[d] == prev_r && goal_c + dcol[d] == prev_c)
                final_dir = d;

        for (int i = len - 1; i > 0; --i) {
            int r1 = path[i][0], c1 = path[i][1];
            int r0 = path[i - 1][0], c0 = path[i - 1][1];
            int dir = -1;
            for (int d = 0; d < 4; ++d)
                if (r1 + drow[d] == r0 && c1 + dcol[d] == c0)
                    dir = d;
            if (dir == final_dir)
                straight_to_goal++;
            else
                break;
        }
        int axis = 0; 
        if (final_dir == 1 || final_dir == 3) 
            axis = 1;
        if (food_axes_ptr)
            food_axes_ptr[current_food_index] = axis;

        food_dists_ptr[current_food_index] = straight_to_goal;
    }

    if (idx < max_instr)
        instr[idx++].type = iSTOP;

    *count = idx;
    *heading_ptr = heading;
}



int generate_instructions_from_map(RobotInstr instr[], int max_instr, int food_dists[], int food_axes[])
{
    int total = 0;
    int cur_r = start_pos[0];
    int cur_c = start_pos[1];
    const int foods = sizeof(food_list) / sizeof(food_list[0]);
    int heading = -1;

    for (int f = 0; f < foods && total < max_instr; ++f) {
        int goal_r = food_list[f][0];
        int goal_c = food_list[f][1];

        if (!is_free(goal_r, goal_c)) {
            food_dists[f] = -1;
            continue;
        }

        int g[ROWS][COLS];
        int prev[ROWS][COLS][2];
        bool closed[ROWS][COLS];
        Node open[ROWS * COLS];
        int osz = 0;

        for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c) {
                g[r][c] = 9999;
                prev[r][c][0] = prev[r][c][1] = -1;
                closed[r][c] = false;
            }

        g[cur_r][cur_c] = 0;
        open[osz++] = (Node){cur_r, cur_c, heuristic(cur_r, cur_c, goal_r, goal_c)};

        while (osz > 0) {
            int best = 0;
            for (int i = 1; i < osz; ++i)
                if (open[i].f < open[best].f)
                    best = i;
            Node n = open[best];
            open[best] = open[--osz];
            if (closed[n.r][n.c]) continue;
            closed[n.r][n.c] = true;
            if (n.r == goal_r && n.c == goal_c) break;

            for (int d = 0; d < 4; ++d) {
                int nr = n.r + drow[d], nc = n.c + dcol[d];
                if (!is_free(nr, nc) || closed[nr][nc]) continue;
                int new_g = g[n.r][n.c] + 1;
                if (new_g < g[nr][nc]) {
                    g[nr][nc] = new_g;
                    prev[nr][nc][0] = n.r;
                    prev[nr][nc][1] = n.c;
                    int fcost = new_g + heuristic(nr, nc, goal_r, goal_c);
                    open[osz++] = (Node){nr, nc, fcost};
                }
            }
        }

        int path[256][2];
        int len = 0;
        int r = goal_r, c = goal_c;
        while (r != -1 && c != -1 && len < 256) {
            path[len][0] = r;
            path[len][1] = c;
            if (r == cur_r && c == cur_c) { len++; break; }
            int pr = prev[r][c][0];
            int pc = prev[r][c][1];
            r = pr; c = pc;
            len++;
        }

        if (len < 2) {
            food_dists[f] = -1;
            continue;
        }

        for (int i = 0; i < len / 2; ++i) {
            int tr = path[i][0], tc = path[i][1];
            path[i][0] = path[len - 1 - i][0];
            path[i][1] = path[len - 1 - i][1];
            path[len - 1 - i][0] = tr;
            path[len - 1 - i][1] = tc;
        }

        int new_heading = -1;
        int r0 = path[0][0], c0 = path[0][1];
        int r1 = path[1][0], c1 = path[1][1];
        for (int d = 0; d < 4; ++d)
            if (r0 + drow[d] == r1 && c0 + dcol[d] == c1)
                new_heading = d;

        if (heading != -1) {
            int back_r = cur_r - drow[heading];
            int back_c = cur_c - dcol[heading];
            if (path[1][0] == back_r && path[1][1] == back_c && total < max_instr) {
                instr[total++].type = iTURN_AROUND;
                heading = new_heading;
            }
        }

        emit_plan_for_path((const int (*)[2])path, len, instr, &total, max_instr, &heading, food_dists, food_axes, f);

        cur_r = goal_r;
        cur_c = goal_c;
    }

    return total;
}
