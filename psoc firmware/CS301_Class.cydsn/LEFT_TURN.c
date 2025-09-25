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
#include "LEFT_TURN.h"

void TURN_LEFT()
{
    PWM_2_WriteCompare(255);
    PWM_1_WriteCompare(0);
    CyDelay(260);
    PWM_2_WriteCompare(127);
    PWM_1_WriteCompare(127);

}