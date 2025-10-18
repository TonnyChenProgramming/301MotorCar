#include "map.h"
#include "map_to_instructions.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>


#define ROWS 15
#define COLS 19

#include <stdio.h>

#ifndef USE_USB
#define USE_USB 1   // set to 0 if you don't want USB printing
#endif

#if USE_USB
extern void usbPutString(char *s);   // declared in main.c
static void dbg_puts(const char *s) { usbPutString((char*)s); }
#else
static void dbg_puts(const char *s) { printf("%s", s); }
#endif

// pretty-print one path (choose ONE_BASED 0 or 1 to taste)
#define ONE_BASED 0

static void debug_print_path(const int path[][2], int len, int leg_idx)
{
    char line[96];

    snprintf(line, sizeof(line), "\r\n=== RAW PATH (leg %d) len=%d ===\r\n", leg_idx, len);
    dbg_puts(line);

    for (int i = 0; i < len; ++i) {
        int r = path[i][0], c = path[i][1];
    #if ONE_BASED
        snprintf(line, sizeof(line), "(%d,%d)%s",
                 r+1, c+1, (i==len-1) ? "\r\n" : " ");
    #else
        snprintf(line, sizeof(line), "(%d,%d)%s",
                 r, c, (i==len-1) ? "\r\n" : " ");
    #endif
        dbg_puts(line);
    }

    // quick validity check: cells must be free and steps adjacent by 1 manhattan
    for (int i = 0; i < len; ++i) {
        int r = path[i][0], c = path[i][1];
        if (r < 0 || r >= ROWS || c < 0 || c >= COLS || map[r][c] != 0) {
            snprintf(line, sizeof(line), "!! invalid cell at i=%d -> (%d,%d)\r\n", i, r, c);
            dbg_puts(line);
        }
        if (i > 0) {
            int dr = path[i][0] - path[i-1][0];
            int dc = path[i][1] - path[i-1][1];
            int man = (dr<0?-dr:dr) + (dc<0?-dc:dc);
            if (man != 1) {
                snprintf(line, sizeof(line), "!! non-adjacent step between i-1=%d and i=%d\r\n", i-1, i);
                dbg_puts(line);
            }
        }
    }

    dbg_puts("=== END RAW PATH ===\r\n");
}


// Directions: 0 = right, 1 = down, 2 = left, 3 = up
static const int drow[4] = {0, 1, 0, -1};
static const int dcol[4] = {1, 0, -1, 0};

static inline bool in_bounds(int r, int c) {
    return (r >= 0 && r < ROWS && c >= 0 && c < COLS);
}
static inline bool is_free(int r, int c) {
    return in_bounds(r, c) && (map[r][c] == 0);
}

// Manhattan heuristic for A*
static inline int heuristic(int r, int c, int gr, int gc) {
    return abs(r - gr) + abs(c - gc);
}

// Check if a cell is an intersection (branching point)
static inline bool is_intersection(int r, int c) {
    int free_dirs = 0;
    for (int d = 0; d < 4; ++d)
        if (is_free(r + drow[d], c + dcol[d]))
            free_dirs++;
    return (free_dirs >= 3); // intersection if 3+ open directions
}

// Node structure for A*
typedef struct {
    int r, c;
    int f;   // f = g + h
} Node;

// Priority queue-like pop (find lowest f)
static int pop_min(Node *arr, int *size) {
    int best = 0;
    for (int i = 1; i < *size; ++i)
        if (arr[i].f < arr[best].f)
            best = i;
    Node tmp = arr[best];
    arr[best] = arr[(*size) - 1];
    arr[(*size) - 1] = tmp;
    (*size)--;
    return (*size);
}

// Emit high-level movement plan (straight segments + turns)
static void emit_plan_for_path(const int path[][2], int len, RobotInstr instr[], int *count, int max_instr)
{
    if (len < 2) return;
    int idx = *count;

    // Determine initial direction
    int r0 = path[0][0], c0 = path[0][1];
    int r1 = path[1][0], c1 = path[1][1];
    int heading = -1;
    for (int d = 0; d < 4; ++d)
        if (r0 + drow[d] == r1 && c0 + dcol[d] == c1)
            heading = d;

    // Begin with a straight move
    if (idx < max_instr)
        instr[idx++].type = iSTRAIGHT;

    // Walk through the path and detect turns only when direction changes
    for (int i = 2; i < len; ++i) {
        int r_prev = path[i - 1][0], c_prev = path[i - 1][1];
        int r_next = path[i][0], c_next = path[i][1];
        int new_dir = -1;
        for (int d = 0; d < 4; ++d)
            if (r_prev + drow[d] == r_next && c_prev + dcol[d] == c_next)
                new_dir = d;

        if (new_dir == -1) continue;

        int diff = (new_dir - heading + 4) & 3;

        // If we change direction, we must have reached an intersection
        if (diff != 0) {
            // Finish the straight segment first
            if (idx < max_instr)
                instr[idx++].type = iSTRAIGHT;

            // Then turn
            if (diff == 1 && idx < max_instr)
                instr[idx++].type = iRIGHT;
            else if (diff == 3 && idx < max_instr)
                instr[idx++].type = iLEFT;
            else if (diff == 2 && idx < max_instr)
                instr[idx++].type = iTURN_AROUND;

            heading = new_dir;
        }
        else if (is_intersection(r_next, c_next)) {
            // Keep straight through intersection
            if (idx < max_instr)
                instr[idx++].type = iSTRAIGHT;
        }

        if (idx >= max_instr - 2)
            break;
    }

    *count = idx;
}

// Main A* function
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

        for (int r = 0; r < ROWS; ++r) {
            for (int c = 0; c < COLS; ++c) {
                g[r][c] = 9999;
                prev[r][c][0] = prev[r][c][1] = -1;
                closed[r][c] = false;
            }
        }

        g[cur_r][cur_c] = 0;
        open[osz++] = (Node){cur_r, cur_c, heuristic(cur_r, cur_c, goal_r, goal_c)};

        // A* search
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

        // Reconstruct path
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

        // Reverse path
        for (int i = 0; i < len / 2; ++i) {
            int tr = path[i][0], tc = path[i][1];
            path[i][0] = path[len - 1 - i][0];
            path[i][1] = path[len - 1 - i][1];
            path[len - 1 - i][0] = tr;
            path[len - 1 - i][1] = tc;
        }
        
        debug_print_path((const int (*)[2])path, len, f);

        // Emit instructions
        emit_plan_for_path((const int (*)[2])path, len, instr, &total, max_instr);

        // Stop at each food
        if (total < max_instr)
            instr[total++].type = iSTOP;

        cur_r = goal_r;
        cur_c = goal_c;
    }

    return total;
}
