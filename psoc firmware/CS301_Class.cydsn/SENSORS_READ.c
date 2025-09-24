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

// Read 6 output pins are turn into a pattern
uint8 ReadSensors(void) {
    uint8 sensorValues = 0;
    
   sensorValues |= Output_1_Read() << 0; // Right
   sensorValues |= Output_2_Read() << 1;
   sensorValues |= Output_3_Read() << 2;
   sensorValues |= Output_4_Read() << 3;
   sensorValues |= Output_5_Read() << 4;
   sensorValues |= Output_6_Read() << 5; // Left
    
   return sensorValues;
}

// Use pattern to decide how the robot should move
MovementState GetMovement(void) {
    uint8 sensors = ReadSensors();
    
    switch(sensors) {
        case 0b001110: return STRAIGHT;
        case 0b001111: return LEFT_TURN;
        case 0b001010: return RIGHT_TURN;
        case 0b000110: return DRIFTED_RIGHT;
        case 0b001000: return DRIFTED_LEFT;
        default:       return STRAIGHT;
    }
}
        