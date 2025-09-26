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
#include <stdint.h>

// States of movement
typedef enum {
    STRAIGHT,
    LEFT_TURN,
    RIGHT_TURN,
    DRIFTED_LEFT,
    DRIFTED_RIGHT,
    STOP,
    WAIT,
} MovementState;

// Prototype
uint8 ReadSensors(void);
MovementState GetMovement(void);
extern uint8 sensorValues;
#endif