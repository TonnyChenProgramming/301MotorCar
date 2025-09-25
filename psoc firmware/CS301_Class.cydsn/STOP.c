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

#include <project.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

void STOP()
{
    PWM_1_WriteCompare(127);
    PWM_2_WriteCompare(127);

}