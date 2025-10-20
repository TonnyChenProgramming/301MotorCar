#ifndef MAP_TO_INSTRUCTIONS_H
#define MAP_TO_INSTRUCTIONS_H

#include <stdint.h>

typedef enum {
    iSTRAIGHT,
    iLEFT,
    iRIGHT,
    iTURN_AROUND,
    iSTOP
} RobotInstrType;

typedef struct {
    RobotInstrType type;
} RobotInstr;

#define MAX_INSTRUCTIONS 256

int generate_instructions_from_map(RobotInstr instr[], int max_instr, int food_dists[], int food_axes[]);



#endif
