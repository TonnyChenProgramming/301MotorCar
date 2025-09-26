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

#include "manoeuvre.h"

void edge_front_left_manoeuvre()
{
    
     
    PWM_1_WriteCompare(178);
    PWM_2_WriteCompare(179);
}
void edge_front_right_manoeuvre()
{
    PWM_1_WriteCompare(174);
    PWM_2_WriteCompare(183);
}
void edge_mid_left_manoeuvre()
{
    PWM_1_WriteCompare(174);
    PWM_2_WriteCompare(183);
}
void edge_mid_right_manoeuvre()
{
    PWM_1_WriteCompare(178);
    PWM_2_WriteCompare(179);
}

void edge_left_wing_manoeuvre()
{
    // Hard left turn
    while (Output_5_Read() == 1 || Output_4_Read() == 1) {
        PWM_2_WriteCompare(157);
        PWM_1_WriteCompare(67);
    } 

}

void edge_right_wing_manoeuvre()
{
    while (Output_5_Read() == 1 || Output_4_Read() == 1)  {
        PWM_1_WriteCompare(150);
        PWM_2_WriteCompare(55);
    }
}
