#include "SENSORS_READ.h"


uint8 sensorValues;
MovementState previous_movement;
MovementState current_movement;
struct MyStructure {   // Structure declaration
    uint8_t output1;
    uint8_t output2;
    uint8_t output3;
    uint8_t output4;
    uint8_t output5;
    uint8_t output6;
};
struct MyStructure this_struct = {0,0,0,0,0,0};
// Read 6 output pins are turn into a pattern
uint8 ReadSensors(void) {
    uint8 sensorValues = 0;
    
   sensorValues |= Output_5_Read() << 0; // Right
   sensorValues |= Output_6_Read() << 1;
   sensorValues |= Output_1_Read() << 2;
   sensorValues |= Output_3_Read() << 3;
   sensorValues |= Output_2_Read() << 4;
   sensorValues |= Output_4_Read() << 5; // Left
    
   return sensorValues;
}

// Call this once at startup somewhere global:
// MovementState previous_movement = STRAIGHT;

MovementState GetMovement(void)
{
    uint8 o1 = Output_1_Read(); // middle-right
    uint8 o2 = Output_2_Read(); // middle-left
    uint8 o3 = Output_3_Read(); // right wing
    uint8 o4 = Output_4_Read(); // front-right
    uint8 o5 = Output_5_Read(); // front-left
    uint8 o6 = Output_6_Read(); // left wing

    // --- 1. If already turning, ignore everything except fronts ---
    if (previous_movement == LEFT_TURN) {
        // Stay in LEFT_TURN until both front sensors see the line
        if ((o5 == 0) && (o4 == 0)) {
            previous_movement = STRAIGHT;
        } else {
            return LEFT_TURN; // ignore right wing, middles, etc.
        }
    }
    else if (previous_movement == RIGHT_TURN) {
        // Stay in RIGHT_TURN until both front sensors see the line
        if ((o5 == 0) && (o4 == 0)) {
            previous_movement = STRAIGHT;
        } else {
            return RIGHT_TURN; // ignore left wing, middles, etc.
        }
    }

    // --- 2. Start turns (only when NOT already turning) ---
    if (o6 == 0) { // left wing active, right wing clear
        previous_movement = LEFT_TURN;
        return LEFT_TURN;
    }
    if (o3 == 0) { // right wing active, left wing clear
        previous_movement = RIGHT_TURN;
        return RIGHT_TURN;
    }

    // --- 3. Both front sensors black → straight ---
    if ((o5 == 0) && (o4 == 0)) {
        previous_movement = STRAIGHT;
        return STRAIGHT;
    }

    // --- 4. Otherwise, keep last movement (usually STRAIGHT) ---
    return previous_movement;
}