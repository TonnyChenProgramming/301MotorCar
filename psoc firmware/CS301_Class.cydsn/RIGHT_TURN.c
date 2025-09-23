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

#include "defines.h"     
#include "vars.h"
#include "isr_eoc2.h"

int main()
{
    CYGlobalIntEnable;
    PWM_1_Start();
    PWM_1_WriteCompare(255);
    CyDelay(1000);   // 1000 ms delay
    PWM_1_Stop();

}