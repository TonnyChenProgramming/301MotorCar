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

/* ---- Movement and Turning ---- */
void move_forward_ticks(int32 steps);
void go_straigh_with_tick(int32 steps);
void move_forward_until_intersection(void);
void move_forward_skip_one_intersection(void);
void turn_left_enc(void);
void turn_right_enc(void);
void u_turn_enc(void);

/* ---- Food Handling ---- */
void run_for_food(uint8_t food_distance, uint8_t axis);


/* ---- Path Execution ---- */
void execute_instruction(uint8_t instr, uint16_t *food_distances);
void execute_path(uint8_t *instructions, uint8_t length, uint16_t *food_distances);

/* ---- Motor Interface ---- */
void motor_left(uint16 val);
void motor_right(uint16 val);

#endif /* MOVEMENT_H */
/* [] END OF FILE */
