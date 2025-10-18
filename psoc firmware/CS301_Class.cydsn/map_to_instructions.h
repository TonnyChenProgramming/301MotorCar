#ifndef MAP_TO_INSTRUCTIONS_H
#define MAP_TO_INSTRUCTIONS_H

#include <stdint.h>
#include "SENSORS_READ.h"
#define MAX_INSTRUCTIONS 256

// Unified movement instruction type

/**
 * Generates a sequence of movement instructions and associated food distances.
 * 
 * @param instr         Output array of MovementState instructions
 * @param max_instr     Maximum number of instructions allowed
 * @param food_dists    Output array storing straight-line distance to each food item
 * @return              Number of generated instructions
 */
int generate_movements_from_map(MovementState instr[], int max_instr, int food_dists[]);

#endif