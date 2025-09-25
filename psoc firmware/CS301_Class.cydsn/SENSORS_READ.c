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
        //case 0b000001: return STRAIGHT;
        case 0b001010: return STRAIGHT;
        case 0b011110: return STRAIGHT;
        case 0b101011: return STRAIGHT;
        
        //find path
        //case 0b111111: return STRAIGHT;
        
        case 0b001000: return LEFT_TURN;
        case 0b011100: return LEFT_TURN;
        case 0b101001: return LEFT_TURN;
        
        case 0b000010: return RIGHT_TURN;
        case 0b010110: return RIGHT_TURN;
        case 0b100011: return RIGHT_TURN;
        
        case 0b111100: return DRIFTED_RIGHT;
        case 0b101010: return DRIFTED_RIGHT;
        case 0b111010: return DRIFTED_RIGHT;
        case 0b111011: return DRIFTED_RIGHT;
        
        case 0b001111: return DRIFTED_LEFT;
        case 0b101111: return DRIFTED_LEFT;
        case 0b001011: return DRIFTED_LEFT;
        
        case 0b111111: return KEEP_RUNNING;
        
        case 0b000000: return STOP;
        default:       return STOP;
    }
}
        