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
    //176 & 181
    PWM_1_WriteCompare(156);
    PWM_2_WriteCompare(161);
    //CyDelay(100);
   // PWM_1_WriteCompare(127);
    //PWM_2_WriteCompare(127);
return;
    
 
}