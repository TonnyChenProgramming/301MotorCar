#include "map.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Instruction types for intersection-based navigation
typedef enum {
    INSTR_FORWARD_UNTIL_INTERSECTION,
    INSTR_TURN_LEFT,
    INSTR_TURN_RIGHT,
    INSTR_GO_STRAIGHT,
    INSTR_SKIP_LEFT,
    INSTR_SKIP_RIGHT,
    INSTR_STOP_FOR_FOOD
} RobotInstrType;

typedef struct {
    RobotInstrType type;
} RobotInstr;

#define MAX_INSTRUCTIONS 256

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
        // Convert path to intersection-based instructions
        int last_dir = cur_dir;
        for (int i = 1; i <= path_len; ++i) {
            int r0 = path[i-1][0], c0 = path[i-1][1];
            int r1 = path[i][0], c1 = path[i][1];
            int step_dir = -1;
            for (int d = 0; d < 4; ++d) {
                if (r0 + dr[d] == r1 && c0 + dc[d] == c1) { step_dir = d; break; }
            }
            if (is_intersection(r1, c1) || (r1 == goal_r && c1 == goal_c)) {
                // At intersection or food
                if (step_dir == last_dir) {
                    instr[total_instr++] = (RobotInstr){INSTR_GO_STRAIGHT};
                } else if ((step_dir - last_dir + 4) % 4 == 1) {
                    instr[total_instr++] = (RobotInstr){INSTR_TURN_RIGHT};
                } else if ((last_dir - step_dir + 4) % 4 == 1) {
                    instr[total_instr++] = (RobotInstr){INSTR_TURN_LEFT};
                }
                last_dir = step_dir;
            } else {
                // Continue forward
                if (i == 1 || step_dir == last_dir) {
                    instr[total_instr++] = (RobotInstr){INSTR_FORWARD_UNTIL_INTERSECTION};
                }
            }
            if (total_instr >= max_instr) break;
        }
        instr[total_instr++] = (RobotInstr){INSTR_STOP_FOR_FOOD};
        cur_r = goal_r; cur_c = goal_c; cur_dir = last_dir;
        if (total_instr >= max_instr) break;
    }
    return total_instr;
}
