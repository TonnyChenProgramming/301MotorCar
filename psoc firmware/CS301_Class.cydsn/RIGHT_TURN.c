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

#include "RIGHT_TURN.h"

void TURN_RIGHT()
{
    PWM_1_WriteCompare(255);
    PWM_2_WriteCompare(0);
    CyDelay(250);
    PWM_1_WriteCompare(127);

}