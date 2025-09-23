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

void RIGHT_TURN()
{
    CYGlobalIntEnable;
    PWM_1_Start();
    PWM_1_WriteCompare(255);
    PWM_1_Stop();

}