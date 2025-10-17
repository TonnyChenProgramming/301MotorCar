#ifndef MAP_TO_INSTRUCTIONS_H
#define MAP_TO_INSTRUCTIONS_H

#include <stdint.h>

// =========================
// Robot Instruction Types
// =========================
typedef enum {
    INSTR_FORWARD_UNTIL_INTERSECTION,
    INSTR_TURN_LEFT,
    INSTR_TURN_RIGHT,
    INSTR_GO_STRAIGHT,
    INSTR_SKIP_LEFT,
    INSTR_SKIP_RIGHT,
    INSTR_STOP_FOR_FOOD
} RobotInstrType;

// =========================
// Robot Instruction Struct
// =========================
typedef struct {
    RobotInstrType type;
} RobotInstr;

// =========================
// Constants
// =========================
#define MAX_INSTRUCTIONS 256

// =========================
// Function Prototype
// =========================
int generate_instructions_from_map(RobotInstr instr[], int max_instr);

#endif // MAP_TO_INSTRUCTIONS_H
