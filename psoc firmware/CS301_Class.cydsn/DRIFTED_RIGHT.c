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


#include "DRIFTED_RIGHT.h"

void DRIFT_LEFT()
{
    CYGlobalIntEnable;
    PWM_1_Start();
    PWM_1_WriteCompare(145);
    CyDelay(260);
    PWM_1_WriteCompare(12);

}