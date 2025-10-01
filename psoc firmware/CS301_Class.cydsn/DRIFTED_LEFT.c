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

void DRIFT_RIGHT()
{
    
    PWM_2_WriteCompare(140);
    PWM_2_WriteCompare(12);

}