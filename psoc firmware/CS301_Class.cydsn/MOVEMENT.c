#include "project.h"
#include <stdbool.h>
#include <stdlib.h>
#include "MOVEMENT.h"
#include "SENSORS_READ.h"
#include "main.h"

// ---------- Motor control ----------
void motor_left(uint16 val)  { PWM_1_WriteCompare(val); }
void motor_right(uint16 val) { PWM_2_WriteCompare(val); }

#define STOP_PWM   127
#define L_FWD_PWM  156
#define L_REV_PWM  96
#define R_FWD_PWM  154
#define R_REV_PWM  94

// ---------- Encoder calibration ----------
#define LEFT_TICKS_90_L   (-118)
#define LEFT_TICKS_90_R   (112)
#define RIGHT_TICKS_90_L  (-118)
#define RIGHT_TICKS_90_R  (92)
#define LEFT_TICKS_180_L  (2 * LEFT_TICKS_90_L)
#define RIGHT_TICKS_180_L (2 * RIGHT_TICKS_90_L)
#define LEFT_TICKS_180_R  (2 * LEFT_TICKS_90_R)
#define RIGHT_TICKS_180_R (2 * RIGHT_TICKS_90_R)

#define ENCODER_LEFT_SIGN  (+1)
#define ENCODER_RIGHT_SIGN (+1)

// ---------- Line-follow PWM ----------
uint8_t left_pwm = 154;
uint8_t right_pwm = 157;
uint8_t drift_left_pwm = 162;
uint8_t drift_right_pwm = 158;

// ============================================================================
// Basic motor control
// ============================================================================
void stop(void)
{
    motor_left(STOP_PWM);
    motor_right(STOP_PWM);
}

void go_straight(void)
{
    // drift right (left sensor white)
    if ((Output_5_Read()) && !(Output_4_Read())) {
        motor_left(drift_right_pwm);
        motor_right(right_pwm);
    }
    // drift left (right sensor white)
    else if (!(Output_5_Read()) && (Output_4_Read())) {
        motor_left(left_pwm);
        motor_right(drift_left_pwm);
    }
    else {
        motor_left(left_pwm);
        motor_right(right_pwm);
    }
}

// ============================================================================
// Encoder-based turns
// ============================================================================
static void turn_left_enc(void)
{
    QuadDec_M1_SetCounter(0);
    QuadDec_M2_SetCounter(0);

    motor_left(L_REV_PWM);
    motor_right(R_FWD_PWM);

    while (1) {
        int32 L = ENCODER_LEFT_SIGN  * QuadDec_M1_GetCounter();
        int32 R = ENCODER_RIGHT_SIGN * QuadDec_M2_GetCounter();
        if (abs(L) >= abs(LEFT_TICKS_90_L) && abs(R) >= abs(RIGHT_TICKS_90_L))
            break;
    }

    stop();
    CyDelay(100);
    go_straight();
}

static void turn_right_enc(void)
{
    QuadDec_M1_SetCounter(0);
    QuadDec_M2_SetCounter(0);

    motor_left(L_FWD_PWM);
    motor_right(R_REV_PWM);

    while (1) {
        int32 L = ENCODER_LEFT_SIGN  * QuadDec_M1_GetCounter();
        int32 R = ENCODER_RIGHT_SIGN * QuadDec_M2_GetCounter();
        if (abs(L) >= abs(LEFT_TICKS_90_R) && abs(R) >= abs(RIGHT_TICKS_90_R))
            break;
    }

    stop();
    CyDelay(100);
    go_straight();
}

static void u_turn_enc(void)
{
    QuadDec_M1_SetCounter(0);
    QuadDec_M2_SetCounter(0);

    motor_left(L_REV_PWM);
    motor_right(R_FWD_PWM);

    while (1) {
        int32 L = ENCODER_LEFT_SIGN  * QuadDec_M1_GetCounter();
        int32 R = ENCODER_RIGHT_SIGN * QuadDec_M2_GetCounter();
        if (abs(L) >= abs(LEFT_TICKS_180_L) && abs(R) >= abs(RIGHT_TICKS_180_L))
            break;
    }

    stop();
    CyDelay(100);
    go_straight();
}

// ============================================================================
// Line following and intersection logic
// ============================================================================
void move_forward_until_intersection(void)
{
    int stable = 0;
    static int turn_cooldown = 0;

    while (stable < 1)
    {
        uint8 o1 = Output_1_Read();
        uint8 o2 = Output_2_Read();
        uint8 o3 = Output_3_Read();
        uint8 o4 = Output_4_Read();
        uint8 o5 = Output_5_Read();
        uint8 o6 = Output_6_Read();

        // cooldown countdown
        if (turn_cooldown > 0)
            turn_cooldown--;

        // center must remain on the line
        /*
        if (o1 != 0 && o2 != 0) {
            stop();
            continue;
        }
        */
        // intersection detection (with cooldown)
        bool left_turn  = false;
        bool right_turn = false;

        if (turn_cooldown == 0) {
            if ((o6 == 0) && (o3 == 0)) {
                turn_cooldown = 85;  // T intersection
                right_turn = true;
            } 
            else if (o6 == 0) {
                LED2_Write(1);
                turn_cooldown = 85;
                left_turn = true;
            } 
            else if (o3 == 0) {
                LED4_Write(1);
                turn_cooldown = 85;
                right_turn = true;
            }
        }

        bool intersection = left_turn || right_turn;
        if (intersection)
                    stable++;
                else
                    stable = 0;
    
        go_straight();
    }
    stable = 0;
    stop();
    CyDelay(500);
}


// ============================================================================
// Execute instruction list
// ============================================================================
void execute_instruction(uint8_t instr)
{
    switch (instr)
    {
        case STRAIGHT:
            move_forward_until_intersection();
            break;

        case LEFT_TURN:
            turn_left_enc();
            break;

        case RIGHT_TURN:
            turn_right_enc();
            break;

      

        case STOP:
        default:
            stop();
            break;
    }
}

// ============================================================================
// Execute full instruction sequence
// ============================================================================
void execute_path(uint8_t *instructions, uint8_t length)
{
    for (uint8_t i = 0; i < length; i++)
    {
        uint8_t current = instructions[i];

        // two consecutive straights = skip one intersection
        
            execute_instruction(current);
       

        if (current == STOP) break;
    }
}
