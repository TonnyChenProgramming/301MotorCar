/* ========================================
 *
 * Encoder (Tachometer) Interface Header
 * ======================================== */

#ifndef TACHO_H
#define TACHO_H

#include <project.h>
#include <stdint.h>

/* Global variables updated by get_position() / get_speed() */
 int16 posL;
 int16 posR;
 int16 speedL;
 int16 speedR;

/* Reads absolute encoder counters and updates posL, posR */
void get_position(void);

/* Computes speed (delta since last call) and updates speedL, speedR */
void get_speed(void);

#endif /* TACHO_H */
/* [] END OF FILE */
