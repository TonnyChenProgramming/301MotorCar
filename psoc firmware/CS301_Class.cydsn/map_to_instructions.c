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

// Helper: check if cell is an intersection (more than 2 free neighbors)
static bool is_intersection(int r, int c) {
    int count = 0; 
    for (int d = 0; d < 4; ++d) {
        int nr = r + dr[d], nc = c + dc[d];
        if (is_free(nr, nc)) count++;
    }
    return count > 2;
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

// Dijkstra's algorithm for shortest path (no priority queue for small grid)
typedef struct { int r, c, prev_dir; } Node;

// Fills instr[] with instructions, returns number of instructions
int generate_instructions_from_map(RobotInstr instr[], int max_instr) {
    int food_count = sizeof(food_list) / sizeof(food_list[0]);
    int total_instr = 0;
    int cur_r = start_pos[0], cur_c = start_pos[1];
    int cur_dir = 1; // Start facing down (S)

    for (int food_idx = 0; food_idx < food_count; ++food_idx) {
        int goal_r = food_list[food_idx][0];
        int goal_c = food_list[food_idx][1];
        // Dijkstra's: dist, prev, dir
        int dist[15][19];
        int prev[15][19][2];
        int from_dir[15][19];
        memset(dist, 0x3f, sizeof(dist));
        memset(prev, -1, sizeof(prev));
        memset(from_dir, -1, sizeof(from_dir));
        dist[cur_r][cur_c] = 0;
        from_dir[cur_r][cur_c] = cur_dir;
        Node queue[15*19];
        int qh = 0, qt = 0;
        queue[qt++] = (Node){cur_r, cur_c, cur_dir};
        while (qh < qt) {
            Node n = queue[qh++];
            for (int d = 0; d < 4; ++d) {
                int nr = n.r + dr[d], nc = n.c + dc[d];
                if (!is_free(nr, nc)) continue;
                if (dist[nr][nc] > dist[n.r][n.c] + 1) {
                    dist[nr][nc] = dist[n.r][n.c] + 1;
                    prev[nr][nc][0] = n.r;
                    prev[nr][nc][1] = n.c;
                    from_dir[nr][nc] = d;
                    queue[qt++] = (Node){nr, nc, d};
                }
            }
        }
        // Reconstruct path
        int path_len = dist[goal_r][goal_c];
        int path[256][2];
        int pr = goal_r, pc = goal_c;
        for (int i = path_len; i >= 0; --i) {
            path[i][0] = pr; path[i][1] = pc;
            int tpr = prev[pr][pc][0], tpc = prev[pr][pc][1];
            pr = tpr; pc = tpc;
        }
        // Convert path to instruction stream
        int last_dir = cur_dir;
        for (int i = 1; i <= path_len; ++i) {
            int r0 = path[i-1][0], c0 = path[i-1][1];
            int r1 = path[i][0], c1 = path[i][1];
            int step_dir = -1;
            for (int d = 0; d < 4; ++d) {
                if (r0 + dr[d] == r1 && c0 + dc[d] == c1) { step_dir = d; break; }
            }
            // At each arrived cell (r1,c1), decide what instruction reflects the structure
            if (r1 == goal_r && c1 == goal_c) {
                // Goal reached: handled after loop with STOP_FOR_FOOD
            } else if (step_dir == last_dir) {
                // Continuing straight: emit skip-left/right for any side branches here
                bool leftAvail = has_left_opt(r1, c1, step_dir);
                bool rightAvail = has_right_opt(r1, c1, step_dir);
                if (leftAvail)  instr[total_instr++] = (RobotInstr){INSTR_SKIP_LEFT};
                if (rightAvail) instr[total_instr++] = (RobotInstr){INSTR_SKIP_RIGHT};
                // No generic FORWARD here; we will only emit FORWARD_UNTIL_INTERSECTION for real T-turns
            } else {
                // Direction change: real turn ahead (T/corner)
                // Emit a single forward-until-intersection before the turn, then the turn
                if (has_straight_opt(r0, c0, last_dir)) {
                    // We were moving along a corridor; now we must turn
                    instr[total_instr++] = (RobotInstr){INSTR_FORWARD_UNTIL_INTERSECTION};
                }
                int diff = (step_dir - last_dir + 4) & 3;
                if (diff == 1) instr[total_instr++] = (RobotInstr){INSTR_TURN_RIGHT};
                else if (diff == 3) instr[total_instr++] = (RobotInstr){INSTR_TURN_LEFT};
                // Update heading
                last_dir = step_dir;
            }
            if (total_instr >= max_instr) break;
        }
        instr[total_instr++] = (RobotInstr){INSTR_STOP_FOR_FOOD};
        cur_r = goal_r; cur_c = goal_c; cur_dir = last_dir;
        if (total_instr >= max_instr) break;
    }
    return total_instr;
}
