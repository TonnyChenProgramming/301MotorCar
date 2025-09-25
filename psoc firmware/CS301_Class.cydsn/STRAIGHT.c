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
#include "STRAIGHT.h"
#include "DRIFTED_LEFT.h"
#include "DRIFTED_RIGHT.h"
#include "LEFT_TURN.h"
void MOVE_STRAIGHT()
{
    if ((Output_1_Read() == 0) && (Output_2_Read() == 0) && (Output_4_Read() == 0) && (Output_5_Read() == 0)) {
        
    PWM_1_WriteCompare(176);
    PWM_2_WriteCompare(180);
    CyDelay(50);
    return;
    }
   else if (Output_5_Read() == 1) {DRIFT_RIGHT();}
else if (Output_4_Read() == 1) {DRIFT_LEFT();}
else TURN_LEFT();
}