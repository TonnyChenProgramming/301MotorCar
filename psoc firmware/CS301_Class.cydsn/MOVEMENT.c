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
/* [] END OF FILE */
void move(MovementState movement)
{
    if(movement == STOP)
    {
        STOP_MOVING();
    }
    else if (movement == DRIFTED_LEFT)
    {
        DRIFT_LEFT();
    }
    else if (movement == DRIFTED_RIGHT)
    {
        DRIFT_RIGHT();
    }
    else if (movement == LEFT_TURN)
    {
        TURN_LEFT();
    }
    else if (movement == RIGHT_TURN)
    {
        TURN_RIGHT();
    }
    else if (movement == STRAIGHT)
    {
        MOVE_STRAIGHT();
    }
    
}