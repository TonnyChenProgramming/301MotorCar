#include "map.h"
#include "map_to_instructions.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define ROWS 15
#define COLS 19

// Direction encoding: 0=E,1=S,2=W,3=N
static const int dr[4] = {0, 1, 0, -1};
static const int dc[4] = {1, 0, -1, 0};

static inline bool is_free(int r, int c) {
    return r >= 0 && r < ROWS && c >= 0 && c < COLS && map[r][c] == 0;
}

static inline int free_neighbors(int r, int c) {
    int count = 0;
    for (int d = 0; d < 4; ++d)
        if (is_free(r + dr[d], c + dc[d])) count++;
    return count;
}

// Basic Dijkstra node
typedef struct { int r, c; } Node;

// Main function: fills instr[] and returns count
int generate_instructions_from_map(RobotInstr instr[], int max_instr) {
    int total_instr = 0;
    int cur_r = start_pos[0], cur_c = start_pos[1];
    int cur_dir = 1; // facing South at start

    for (int f = 0; f < 5; ++f) {
        int goal_r = food_list[f][0];
        int goal_c = food_list[f][1];

        int dist[ROWS][COLS];
        int prev[ROWS][COLS][2];
        memset(dist, 0x3f, sizeof(dist));
        memset(prev, -1, sizeof(prev));

        Node q[ROWS * COLS];
        int qh = 0, qt = 0;

        dist[cur_r][cur_c] = 0;
        q[qt++] = (Node){cur_r, cur_c};

        while (qh < qt) {
            Node n = q[qh++];
            for (int d = 0; d < 4; ++d) {
                int nr = n.r + dr[d], nc = n.c + dc[d];
                if (!is_free(nr, nc)) continue;
                if (dist[nr][nc] > dist[n.r][n.c] + 1) {
                    dist[nr][nc] = dist[n.r][n.c] + 1;
                    prev[nr][nc][0] = n.r;
                    prev[nr][nc][1] = n.c;
                    q[qt++] = (Node){nr, nc};
                }
            }
        }

        // reconstruct path
        int path_len = dist[goal_r][goal_c];
        if (path_len <= 0 || path_len > 255) continue;
        int path[256][2];
        int r = goal_r, c = goal_c;
        for (int i = path_len; i >= 0; --i) {
            path[i][0] = r;
            path[i][1] = c;
            int pr = prev[r][c][0];
            int pc = prev[r][c][1];
            r = pr; c = pc;
        }

        // convert path to instructions
        int heading = cur_dir;
        for (int i = 1; i <= path_len; ++i) {
            int r0 = path[i - 1][0], c0 = path[i - 1][1];
            int r1 = path[i][0], c1 = path[i][1];
            int new_dir = -1;

            for (int d = 0; d < 4; ++d)
                if (r0 + dr[d] == r1 && c0 + dc[d] == c1)
                    new_dir = d;

            int diff = (new_dir - heading + 4) & 3;
            int branches = free_neighbors(r1, c1);

            if (branches >= 4) {
                instr[total_instr++] = (RobotInstr){INSTR_SKIP_4WAY};
            } else if (diff == 1) {
                instr[total_instr++] = (RobotInstr){INSTR_TAKE_RIGHT};
            } else if (diff == 3) {
                instr[total_instr++] = (RobotInstr){INSTR_TAKE_LEFT};
            } else if (diff == 0) {
                // straight; check optional side paths
                int left = (heading + 3) & 3;
                int right = (heading + 1) & 3;
                if (is_free(r1 + dr[left], c1 + dc[left]))
                    instr[total_instr++] = (RobotInstr){INSTR_SKIP_LEFT};
                if (is_free(r1 + dr[right], c1 + dc[right]))
                    instr[total_instr++] = (RobotInstr){INSTR_SKIP_RIGHT};
            }

            heading = new_dir;
            if (total_instr >= max_instr - 1) break;
        }

        instr[total_instr++] = (RobotInstr){INSTR_STOP_FOR_FOOD};

        cur_r = goal_r;
        cur_c = goal_c;
        cur_dir = heading;
        if (total_instr >= max_instr) break;
    }

    return total_instr;
}