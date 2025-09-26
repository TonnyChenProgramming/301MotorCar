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
    while (Output_5_Read() == 1 || Output_4_Read() == 1) {
    PWM_2_WriteCompare(167);
    PWM_1_WriteCompare(57);
    }
    

}