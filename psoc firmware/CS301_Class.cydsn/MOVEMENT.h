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
    
void stop(void);


// Follow line until STOP or intersection
void move_handling(MovementState m);
void do_straight_with_pid(uint16_t o1, uint16_t o2, uint16_t o3,
                          uint16_t o4, uint16_t o5, uint16_t o6);
#endif
/* [] END OF FILE */
