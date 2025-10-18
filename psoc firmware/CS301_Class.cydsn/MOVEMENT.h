/* ========================================
 *
 * Movement Control Header
 * ======================================== */

#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <project.h>
#include <stdbool.h>
#include <stdint.h>

/* ---- Basic Motion ---- */
void go_straight(void);
void stop(void);
void execute_path(uint8_t *instructions, uint8_t length);
/* ---- Turn Handling ---- */
void turn_left_until_line(void);
void turn_right_until_line(void);

/* ---- Path Progression ---- */
void move_forward_until_intersection(void);
void move_until_left_turn(void);
void move_until_right_turn(void);

/* ---- State Machine Integration ---- */
void move_handling(void);

/* ---- Line / Junction Detection ---- */
bool is_at_intersection(void);
bool on_line(void);

/* ---- Motor Interface ----
 * motor_left() and motor_right() are globally visible
 * wrappers to PWM compare writes.
 */
void motor_left(uint16 val);
void motor_right(uint16 val);
void turn_right_enc(void);
void turn_left_enc(void);
void move_forward_ticks(int32 steps);
#endif /* MOVEMENT_H */
/* [] END OF FILE */
