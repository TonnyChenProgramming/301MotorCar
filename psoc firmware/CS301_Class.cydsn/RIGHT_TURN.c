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
   while (Output_4_Read() == 1) {
    PWM_1_WriteCompare(167);
    PWM_2_WriteCompare(87);
    }
    PWM_2_WriteCompare(127);
    PWM_1_WriteCompare(127);

}