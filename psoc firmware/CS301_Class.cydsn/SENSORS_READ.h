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


#ifndef SENSORS_READ_H
#define SENSORS_READ_H

#include <project.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// States of movement
typedef enum {
    STRAIGHT,
    TURN_LEFT,
    TURN_RIGHT,
    DRIFTED_LEFT,
    DRIFTED_RIGHT
} MovementState;

// Prototype
uint8 ReadSensors(void);
MovementState GetMovement(void);

#endif