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

#include "STRAIGHT.h"
#include "STOP.h"
#include "DRIFTED_LEFT.h"
#include "DRIFTED_RIGHT.h"
#include "LEFT_TURN.h"
#include "RIGHT_TURN.h"
#include "SENSORS_READ.h"

uint8_t last_memeory = 0;
/* [] END OF FILE */
void move(MovementState movement)
{

    if (movement == LEFT_TURN)
    {
        TURN_LEFT();
        last_memeory = 0;
    }
    else if (movement == RIGHT_TURN)
    {
        TURN_RIGHT();
        last_memeory = 0;
    }
    
}
void move2(MovementState movement)
{
    if(movement == STOP)
    {
        STOP_MOVING();
        last_memeory = 0;
    }
    else if (movement == DRIFTED_LEFT)
    {
        DRIFT_RIGHT();
        last_memeory = 1;
    }
    else if (movement == DRIFTED_RIGHT)
    {
        DRIFT_LEFT();
        last_memeory = 2;
        
    }
    else if (movement == LEFT_TURN)
    {
        TURN_LEFT();
        last_memeory = 0;
    }
    else if (movement == RIGHT_TURN)
    {
        TURN_RIGHT();
        last_memeory = 0;
    }
    else if (movement == STRAIGHT)
    {
        MOVE_STRAIGHT();
        last_memeory = 0;
    }
/*
    else if (movement == KEEP_RUNNING)
    {
        if (last_memeory == 0)
        {
            
        STOP_MOVING();
        } else if (last_memeory == 1)
        {
             DRIFT_RIGHT();
        } else if (last_memeory == 2)
        {
            DRIFT_LEFT();
        }
    }
    */
    
}