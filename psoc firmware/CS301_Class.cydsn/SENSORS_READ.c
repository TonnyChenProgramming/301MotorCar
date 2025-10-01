/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

/* [] END OF FILE */

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
   
   //this_struct.output1 = Output_1_Read();
    //this_struct.output2 = Output_2_Read();
    //this_struct.output3 = Output_3_Read();
    //this_struct.output4 = Output_4_Read();
    //this_struct.output5 = Output_5_Read();
    //this_struct.output6 = Output_6_Read();
   return sensorValues;
}

// Use pattern to decide how the robot should move
MovementState GetMovement(void) {
    uint8 sensors = ReadSensors();
    switch(sensors) {
            case 0b001010: current_movement = STRAIGHT; break;
    case 0b011110: current_movement = STRAIGHT; break;
    case 0b101011: current_movement = STRAIGHT; break;
    case 0b111111: current_movement = STRAIGHT; break;  
    
    //case 0b101010: current_movement = STRAIGHT; break;
    //case 0b001011: current_movement = STRAIGHT; break;

    case 0b001000: current_movement = LEFT_TURN; break;
    case 0b011100: current_movement = LEFT_TURN; break;
    case 0b101001: current_movement = LEFT_TURN; break;

    case 0b000010: current_movement = RIGHT_TURN; break;
    case 0b010110: current_movement = RIGHT_TURN; break;
    case 0b100011: current_movement = RIGHT_TURN; break;

    case 0b111100: current_movement = DRIFTED_RIGHT; break;
    case 0b101010: current_movement = DRIFTED_RIGHT; break;
    case 0b111010: current_movement = DRIFTED_RIGHT; break;
    case 0b111011: current_movement = DRIFTED_RIGHT; break;

    case 0b001111: current_movement = DRIFTED_LEFT; break;
    case 0b101111: current_movement = DRIFTED_LEFT; break;
    case 0b001011: current_movement = DRIFTED_LEFT; break;

    case 0b000000: current_movement = STOP; break;

    default: current_movement = STOP; break;
    }
    
    // during turning, do nothing
    if ((previous_movement == LEFT_TURN || previous_movement == RIGHT_TURN) && current_movement != STRAIGHT)
    {
        return WAIT;
    }
    
    previous_movement = current_movement;
    return current_movement;
}
        