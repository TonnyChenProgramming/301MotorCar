#include "map.h"
#include "map_to_instructions.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define ROWS 15
#define COLS 19

// Debug printing to USB or console
#ifndef USE_USB
#define USE_USB 1
#endif

#if USE_USB
extern void usbPutString(char *s);
static void dbg_puts(const char *s) { usbPutString((char*)s); }
#else
static void dbg_puts(const char *s) { printf("%s", s); }
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

// Debug print A* path to serial for verification
static void debug_print_path(const int path[][2], int len, int leg_idx) {
    char line[96];
    snprintf(line, sizeof(line), "\r\n=== RAW PATH (leg %d) len=%d ===\r\n", leg_idx, len);
    dbg_puts(line);

    for (int i = 0; i < len; ++i) {
        snprintf(line, sizeof(line), "(%d,%d)%s", path[i][0], path[i][1],
                 (i == len - 1) ? "\r\n" : " ");
        dbg_puts(line);
    }

    dbg_puts("=== END RAW PATH ===\r\n");
}

// Node for A* open list
typedef struct {
    int r, c, f;
} Node;

// Priority queue pop (min f)
static int pop_best(Node *arr, int *size) {
    int best = 0;
    for (int i = 1; i < *size; ++i)
        if (arr[i].f < arr[best].f) best = i;
    Node temp = arr[best];
    arr[best] = arr[--(*size)];
    arr[*size] = temp;
    return (*size);
}

// Put near other helpers
static inline bool has_side_branch(int r, int c, int came_dir, int go_dir) {
    for (int d = 0; d < 4; ++d) {
        if (!is_free(r + drow[d], c + dcol[d])) continue;
        if (d != came_dir && d != go_dir) return true;
    }
    return false;
}

static void emit_plan_for_path(const int path[][2], int len,
                               RobotInstr instr[], int *count, int max_instr)
{
    if (len < 2) return;
    int idx = *count;

    // infer initial heading from first step
    int r0 = path[0][0], c0 = path[0][1];
    int r1 = path[1][0], c1 = path[1][1];
    int heading = -1;
    for (int d = 0; d < 4; ++d)
        if (r0 + drow[d] == r1 && c0 + dcol[d] == c1)
            heading = d;

    // count decision points (intersections) passed while keeping same heading
    int straight_decisions = 0;

    for (int i = 1; i < len; ++i) {
        int r_prev = path[i-1][0], c_prev = path[i-1][1];
        int r_here = path[i][0],  c_here = path[i][1];

        // direction for this step
        int step_dir = -1;
        for (int d = 0; d < 4; ++d)
            if (r_prev + drow[d] == r_here && c_prev + dcol[d] == c_here)
                step_dir = d;

        int came_dir = (step_dir + 2) & 3;
        int go_dir   = step_dir;

        bool is_goal = (i == len - 1);
        bool decision_point = is_goal || has_side_branch(r_here, c_here, came_dir, go_dir);

        int diff = (step_dir - heading + 4) & 3;

        if (diff == 0) {
            // still going same way
            if (decision_point) {
                // passed a junction while straight
                straight_decisions++;
            }
        } else {
            // heading changes at this cell -> turn
            // If this cell is also a decision point, the straight segment
            // should count it as well (we reached this intersection before turning).
            int pre_straights = straight_decisions + (decision_point ? 1 : 0);

            for (int k = 0; k < pre_straights && idx < max_instr; ++k)
                instr[idx++].type = iSTRAIGHT;

            if (diff == 1 && idx < max_instr)      instr[idx++].type = iRIGHT;
            else if (diff == 3 && idx < max_instr) instr[idx++].type = iLEFT;
            else if (diff == 2 && idx < max_instr) instr[idx++].type = iTURN_AROUND;

            heading = step_dir;
            straight_decisions = 0;

            if (idx >= max_instr - 3) break;
        }
    }

    // end of path (goal): emit any remaining STRAIGHTs
    for (int k = 0; k < straight_decisions && idx < max_instr; ++k)
        instr[idx++].type = iSTRAIGHT;

    *count = idx;
}


// Main A* and instruction generator
int generate_instructions_from_map(RobotInstr instr[], int max_instr)
{
    int total = 0;
    int cur_r = start_pos[0];
    int cur_c = start_pos[1];
    const int foods = sizeof(food_list) / sizeof(food_list[0]);

    for (int f = 0; f < foods && total < max_instr; ++f) {
        int goal_r = food_list[f][0];
        int goal_c = food_list[f][1];

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

        // --- A* search loop ---
        while (osz > 0) {
            int best = 0;
            for (int i = 1; i < osz; ++i)
                if (open[i].f < open[best].f)
                    best = i;
            Node n = open[best];
            open[best] = open[--osz];
            if (closed[n.r][n.c]) continue;
            closed[n.r][n.c] = true;
            if (n.r == goal_r && n.c == goal_c)
                break;

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

        // Reconstruct shortest path
        int path[256][2];
        int len = 0;
        int r = goal_r, c = goal_c;
        while (r != -1 && c != -1 && len < 256) {
            path[len][0] = r;
            path[len][1] = c;
            if (r == cur_r && c == cur_c) {
                len++;
                break;
            }
            int pr = prev[r][c][0];
            int pc = prev[r][c][1];
            r = pr; c = pc;
            len++;
        }
        if (len < 2) break;

        // Reverse path order
        for (int i = 0; i < len / 2; ++i) {
            int tr = path[i][0], tc = path[i][1];
            path[i][0] = path[len - 1 - i][0];
            path[i][1] = path[len - 1 - i][1];
            path[len - 1 - i][0] = tr;
            path[len - 1 - i][1] = tc;
        }

        // Print debug path
        debug_print_path(path, len, f);

        // Emit human-level movement plan
        emit_plan_for_path((const int (*)[2])path, len, instr, &total, max_instr);

        // Stop at food
        if (total < max_instr)
            instr[total++].type = iSTOP;

        cur_r = goal_r;
        cur_c = goal_c;
    }

    return total;
}
