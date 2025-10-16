#include "SENSORS_READ.h"

uint8 sensorValues;
MovementState previous_movement;
MovementState current_movement;
static uint8 turn_cooldown = 0;

MovementState GetMovement(void)
{
    uint8 o3 = Output_3_Read(); // right wing
    uint8 o6 = Output_6_Read(); // left wing
    uint8 o4 = Output_4_Read(); // front-right
    uint8 o5 = Output_5_Read(); // front-left
    
   // static uint8 right_on = 0;
   // static uint8 left_on = 0;
   // static MovementState current_state = STRAIGHT;
    
    // First: clear all LEDs every loop to avoid ghosting
   
    if (turn_cooldown > 0)
        turn_cooldown--;
    
    LED1_Write(0);
    LED2_Write(0);
    LED3_Write(0);
    LED4_Write(0);
    LED5_Write(0);
    LED6_Write(0);
    
    // --- Wing sensors ---
    
    if (turn_cooldown == 0) 
    {
        
    if ((o6 == 0) && (o3 == 0)) {
        turn_cooldown = 70;
        return RIGHT_TURN;
    }
    
    // Left Wing
    if (o6 == 0) {
        LED2_Write(1);
        
        /*
        if (left_on < 5) {
            left_on ++;
        } else {
            left_on = 0;
        }
        */
        
        turn_cooldown = 70;
        return LEFT_TURN;
    }
    
    // Right Wing
    if (o3 == 0) {
        LED4_Write(1); 
        
        /*
        if (right_on < 5) {
           right_on ++;
        } else {
            right_on = 0;
        }
        */
        turn_cooldown = 70; //25
        return RIGHT_TURN;
    }
    }
    
   // if (right_on >= 5)
    
    // --- Front sensors ---
    
    // front-left & front right
    if (o5 == 0 && o4 == 0) {
        LED1_Write(1); 
        LED6_Write(1);
        
        return STRAIGHT;
    }
    

    return STRAIGHT;
}
