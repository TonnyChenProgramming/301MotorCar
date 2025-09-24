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

#include "DRIFTED_LEFT.h"

void DRIFTED_LEFT()
{
    CYGlobalIntEnable;
    PWM_2_Start();
    PWM_2_WriteCompare(145);
    CyDelay(260);
    PWM_2_WriteCompare(12);

}