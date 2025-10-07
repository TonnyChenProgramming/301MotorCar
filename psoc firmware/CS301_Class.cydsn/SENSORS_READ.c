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


MovementState GetMovement(void)
{
    uint8 sensors = ReadSensors();

    // Wing sensors first → trigger hard turns
    if (!Output_6_Read()) {   // left wing active
        previous_movement = LEFT_TURN;
        return LEFT_TURN;
    }
    else if (!Output_3_Read()) {  // right wing active
        previous_movement = RIGHT_TURN;
        return RIGHT_TURN;
    }

    // If we were turning, but middle sensors are not back yet → WAIT
    if ((previous_movement == LEFT_TURN || previous_movement == RIGHT_TURN)) 
    {
        // Check middle sensors
        if (Output_1_Read() && Output_2_Read()) {
            return WAIT;  // stay in WAIT until line reacquired
        } else {
            previous_movement = STRAIGHT; // line back under middle sensors
        }
    }

    previous_movement = STRAIGHT;
    return STRAIGHT;
}
