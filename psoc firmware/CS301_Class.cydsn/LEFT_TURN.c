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

void LEFT_TURN()
{
    CYGlobalIntEnable;
    PWM_2_Start();
    PWM_2_WriteCompare(255);
    PWM_2_Stop();

}