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
    uint8 o3 = Output_3_Read(); // right wing
    uint8 o6 = Output_6_Read(); // left wing
    uint8 o4 = Output_4_Read(); // front-right
    uint8 o5 = Output_5_Read(); // front-left
    
    // First: clear all LEDs every loop to avoid ghosting
    LED1_Write(0);
    LED2_Write(0);
    LED3_Write(0);
    LED4_Write(0);
    LED5_Write(0);
    LED6_Write(0);
    
    // --- Wing sensors ---
    
    // Left Wing
    if (o6 == 0) {
        LED2_Write(1);
        
        return LEFT_TURN;
    }
    
    // Right Wing
    if (o3 == 0) {
        LED4_Write(1); 
        
        return RIGHT_TURN;
    }
    
    // --- Front sensors ---
    
    // front-left & front right
    if (o5 == 0 || o4 == 0) {
        LED1_Write(1); 
        LED6_Write(1);
        
        return STRAIGHT;
    }


    return STRAIGHT;
}
