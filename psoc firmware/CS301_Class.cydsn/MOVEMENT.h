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

#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "SENSORS_READ.h"
#include <stdbool.h>
    
void stop(void);
void go_straight(void);

// Minimal helpers for executing high-level instructions
bool is_at_intersection(void);
bool on_line(void);
void move_forward_until_intersection(void);
void turn_left_until_line(void);
void turn_right_until_line(void);


// Follow line until STOP or intersection
void move_handling(void);

#endif
/* [] END OF FILE */
