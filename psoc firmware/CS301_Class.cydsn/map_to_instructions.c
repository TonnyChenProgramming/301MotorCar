#include "map.h"
#include "map_to_instructions.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Direction: 0=E, 1=S, 2=W, 3=N
static const int dr[4] = {0, 1, 0, -1};
static const int dc[4] = {1, 0, -1, 0};

// Helper: check if cell is in bounds and not a wall
static bool is_free(int r, int c) {
    return r >= 0 && r < 15 && c >= 0 && c < 19 && map[r][c] == 0;
}

// Relative branch availability from a cell given heading (0=E,1=S,2=W,3=N)
static inline bool has_straight_opt(int r, int c, int heading) {
    return is_free(r + dr[heading], c + dc[heading]);
}
static inline bool has_left_opt(int r, int c, int heading) {
    int ld = (heading + 3) & 3;
    return is_free(r + dr[ld], c + dc[ld]);
}
static inline bool has_right_opt(int r, int c, int heading) {
    int rd = (heading + 1) & 3;
    return is_free(r + dr[rd], c + dc[rd]);
}

static inline int dir_from_step(int r0, int c0, int r1, int c1) {
    for (int d = 0; d < 4; ++d) {
        if (r0 + dr[d] == r1 && c0 + dc[d] == c1) return d;
    }
    return -1; // invalid
}

static inline void try_emit(RobotInstr instr[], int max_instr, int *count, RobotInstrType t) {
    if (*count < max_instr) {
        instr[*count].type = t;
        (*count)++;
    }
}

// Dijkstra's algorithm for shortest path (queue works for unit weights)
typedef struct { int r, c; } Node;

// Fills instr[] with instructions, returns number of instructions
int generate_instructions_from_map(RobotInstr instr[], int max_instr) {
    int food_count = sizeof(food_list) / sizeof(food_list[0]);
    int total_instr = 0;

    // current pose
    int cur_r = start_pos[0], cur_c = start_pos[1];
    int cur_dir = 1; // provisional (South). Will snap to first move of first leg.

    for (int food_idx = 0; food_idx < food_count; ++food_idx) {
        int goal_r = food_list[food_idx][0];
        int goal_c = food_list[food_idx][1];

        // Dijkstra: dist and prev
        int dist[15][19];
        int prev_r[15][19], prev_c[15][19];
        memset(dist, 0x3f, sizeof(dist));
        for (int r = 0; r < 15; ++r) for (int c = 0; c < 19; ++c) { prev_r[r][c] = -1; prev_c[r][c] = -1; }

        Node q[15*19];
        int qh = 0, qt = 0;
        dist[cur_r][cur_c] = 0;
        q[qt++] = (Node){cur_r, cur_c};

        while (qh < qt) {
            Node n = q[qh++];
            int d0 = dist[n.r][n.c] + 1;
            for (int d = 0; d < 4; ++d) {
                int nr = n.r + dr[d], nc = n.c + dc[d];
                if (!is_free(nr, nc)) continue;
                if (dist[nr][nc] > d0) {
                    dist[nr][nc] = d0;
                    prev_r[nr][nc] = n.r;
                    prev_c[nr][nc] = n.c;
                    q[qt++] = (Node){nr, nc};
                }
            }
        }

        if (dist[goal_r][goal_c] >= 0x3f3f3f3f) {
            // No path: just stop-for-food to avoid empty loop
            try_emit(instr, max_instr, &total_instr, INSTR_STOP_FOR_FOOD);
            cur_r = goal_r; cur_c = goal_c; // advance anyway
            continue;
        }

        // Reconstruct path (start..goal) into path[0..L]
        int L = dist[goal_r][goal_c];
        int path_r[256], path_c[256];
        int pr = goal_r, pc = goal_c;
        for (int i = L; i >= 0; --i) {
            path_r[i] = pr; path_c[i] = pc;
            int tpr = prev_r[pr][pc], tpc = prev_c[pr][pc];
            pr = tpr; pc = tpc;
        }

        // Snap initial heading on first leg to match rule:
        // If first move is downward, face South; otherwise face East.
        if (food_idx == 0 && L >= 1) {
            int first_dir = dir_from_step(path_r[0], path_c[0], path_r[1], path_c[1]);
            cur_dir = (first_dir == 1) ? 1 : 0;
        }

        // Walk the path and emit junction decisions only
        for (int i = 0; i < L; ++i) {
            int r0 = path_r[i], c0 = path_c[i];
            int r1 = path_r[i+1], c1 = path_c[i+1];

            int step_dir = dir_from_step(r0, c0, r1, c1);
            if (step_dir < 0) continue; // safety

            // Branch availability at the current cell, relative to current heading
            bool hasS = has_straight_opt(r0, c0, cur_dir);
            bool hasL = has_left_opt(r0, c0, cur_dir);
            bool hasR = has_right_opt(r0, c0, cur_dir);
            bool fourWay = hasS && hasL && hasR;

            // Decide instruction based on relative move and options present
            if (step_dir == cur_dir) {
                // Going straight
                if (fourWay) {
                    try_emit(instr, max_instr, &total_instr, INSTR_SKIP_4WAY);
                } else if (hasL) {
                    try_emit(instr, max_instr, &total_instr, INSTR_SKIP_LEFT);
                } else if (hasR) {
                    try_emit(instr, max_instr, &total_instr, INSTR_SKIP_RIGHT);
                } else {
                    // pure corridor: no instruction
                }
            } else if (step_dir == ((cur_dir + 3) & 3)) {
                // Left turn
                try_emit(instr, max_instr, &total_instr, INSTR_TAKE_LEFT);
            } else if (step_dir == ((cur_dir + 1) & 3)) {
                // Right turn
                try_emit(instr, max_instr, &total_instr, INSTR_TAKE_RIGHT);
            } else {
                // U-turn (shouldn't happen on optimal grid paths). Ignore.
            }

            // Advance heading
            cur_dir = step_dir;
        }

        // Arrived at food: pause
        try_emit(instr, max_instr, &total_instr, INSTR_STOP_FOR_FOOD);

        // Advance to next leg
        cur_r = goal_r; cur_c = goal_c;
    }

    return total_instr;
}
