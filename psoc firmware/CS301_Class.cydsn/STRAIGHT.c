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

void MOVE_STRAIGHT()
{
    PWM_1_Start();
    PWM_2_Start();
    PWM_1_WriteCompare(210);
    PWM_2_WriteCompare(210);
    CyDelay(1000);
    PWM_1_WriteCompare(127);
    PWM_2_WriteCompare(127);
    

}