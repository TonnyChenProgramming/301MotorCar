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
#define L_FWD_PWM  150
#define L_REV_PWM  96
#define R_FWD_PWM  154
#define R_REV_PWM  94

// ---------- Encoder calibration ----------
//left turn
#define LEFT_TICKS_90_L   (-90) //left motor during left
#define RIGHT_TICKS_90_L  (-100) //right motor during left turn

//right turn
#define LEFT_TICKS_90_R   (100) //left motor during right turn
#define RIGHT_TICKS_90_R  (90) //right motor during right 90

#define LEFT_TICKS_180_L  (195)
#define RIGHT_TICKS_180_L (210)

#define ENCODER_LEFT_SIGN  (+1)
#define ENCODER_RIGHT_SIGN (+1)

#define COUNTDOWN_LEFT 0
#define COUNTDOWN_RIGHT 0
// ---------- Line-follow PWM ----------
uint8_t left_pwm = 154;
uint8_t right_pwm = 157;
uint8_t drift_left_pwm = 162;
uint8_t drift_right_pwm = 158;
//food number tracking
static uint8_t food_index = 0;
static uint16_t count_down = 0;
uint8_t last_drift = 0;

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
        last_drift = 0;
    }
    // drift left (right sensor white)
    else if (!(Output_5_Read()) && (Output_4_Read())) {
        motor_left(left_pwm);
        motor_right(drift_left_pwm);
        last_drift = 1;
    }
    else if ((Output_5_Read()) && (Output_4_Read())) {
        if (last_drift) {
            motor_left(left_pwm);
            motor_right(drift_left_pwm);
        }
        else {
        motor_left(drift_right_pwm);
        motor_right(right_pwm);}
    }
    else
        {
        motor_left(left_pwm);
        motor_right(right_pwm);
        }

    }


// ============================================================================
// Encoder-based turns
// ============================================================================
void move_forward_ticks(int32 steps)
{
    QuadDec_M1_SetCounter(0);
    QuadDec_M2_SetCounter(0);

    motor_left(L_FWD_PWM);
    motor_right(R_FWD_PWM);

    while (1)
    {
        int32 L = ENCODER_LEFT_SIGN  * QuadDec_M1_GetCounter();
        int32 R = ENCODER_RIGHT_SIGN * QuadDec_M2_GetCounter();

        if (abs(L) >= steps && abs(R) >= steps)
            break;
    }

    stop();
    CyDelay(50);
}
// ============================================================================
// Encoder-based turns + Basic motor control
// ============================================================================
void go_straigh_with_tick(int32 steps)
{
    QuadDec_M1_SetCounter(0);
    QuadDec_M2_SetCounter(0);
    
    while (1)
    {
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
        int32 L = ENCODER_LEFT_SIGN  * QuadDec_M1_GetCounter();
        int32 R = ENCODER_RIGHT_SIGN * QuadDec_M2_GetCounter();

        if (abs(L) >= steps && abs(R) >= steps)
            break;
    }
}

/* -------------------- LEFT TURN -------------------- */
void turn_left_enc(void)
{
    // Small forward movement before turning
    move_forward_ticks(10);

    // Reset encoders for the actual turn
    QuadDec_M1_SetCounter(0);
    QuadDec_M2_SetCounter(0);

    motor_left(L_REV_PWM);
    motor_right(R_FWD_PWM);

    bool left_done = false;
    bool right_done = false;

    while (!(left_done && right_done))
    {
        int32 L = ENCODER_LEFT_SIGN  * QuadDec_M1_GetCounter();
        int32 R = ENCODER_RIGHT_SIGN * QuadDec_M2_GetCounter();

        if (!left_done && abs(L) >= abs(LEFT_TICKS_90_L)) {
            motor_left(STOP_PWM);
            left_done = true;
        }

        if (!right_done && abs(R) >= abs(RIGHT_TICKS_90_L)) {
            motor_right(STOP_PWM);
            right_done = true;
        }
    }
    // SMALL adjustment if undershoot
    motor_left(100);
    motor_right(152);
    count_down = 0;
    if ((Output_4_Read() == 1) && (Output_5_Read() == 1)) {
        count_down = COUNTDOWN_LEFT;
    }
    while (count_down > 0)
    {
        count_down--;
    }

    stop();
    CyDelay(100);
}

/* -------------------- RIGHT TURN -------------------- */
void turn_right_enc(void)
{
    // Small forward movement before turning
    move_forward_ticks(20);

    // Reset encoders for the actual turn
    QuadDec_M1_SetCounter(0);
    QuadDec_M2_SetCounter(0);

    motor_left(L_FWD_PWM);
    motor_right(R_REV_PWM);

    bool left_done = false;
    bool right_done = false;

    while (!(left_done && right_done))
    {
        int32 L = ENCODER_LEFT_SIGN  * QuadDec_M1_GetCounter();
        int32 R = ENCODER_RIGHT_SIGN * QuadDec_M2_GetCounter();

        if (!left_done && abs(L) >= abs(LEFT_TICKS_90_R)) {
            motor_left(STOP_PWM);
            left_done = true;
        }

        if (!right_done && abs(R) >= abs(RIGHT_TICKS_90_R)) {
            motor_right(STOP_PWM);
            right_done = true;
        }

    }
    // SMALL adjustment if undershoot
    count_down = 0;
    motor_left(150); //170, 80
    motor_right(98);// 154, 90
            
    if ((Output_5_Read() == 1) && (Output_4_Read() == 1)) {

        count_down = COUNTDOWN_RIGHT;
    }
    while (count_down > 0){
        count_down--;
    }

    stop();
    CyDelay(100);
}



 void u_turn_enc(void)
{
    QuadDec_M1_SetCounter(0);
    QuadDec_M2_SetCounter(0);
    
    
    motor_left(L_REV_PWM);
    motor_right(STOP_PWM);
    while (1) {
        int32 L = ENCODER_LEFT_SIGN  * QuadDec_M1_GetCounter();
        
        if (abs(L) >= abs(LEFT_TICKS_180_L) )
            break;
    }
    motor_left(STOP_PWM);
    motor_right(R_FWD_PWM);
    while (1) {

        int32 R = ENCODER_RIGHT_SIGN * QuadDec_M2_GetCounter();
        if (abs(R) >= abs(RIGHT_TICKS_180_L))
            break;
    }

    stop();
    CyDelay(100);
    go_straight();

   
}

// ============================================================================
// Line following and intersection logic
// ============================================================================


/*
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

        // intersection detection (with cooldown)
        bool left_turn  = false;
        bool right_turn = false;

        if (turn_cooldown == 0) {
            if ((o6 == 0) && (o3 == 0)) {
                turn_cooldown = 20000;  // T intersection
                right_turn = true;
            } 
            else if (o6 == 0) {
                LED2_Write(1);
                turn_cooldown = 20000;
                left_turn = true;
            } 
            else if (o3 == 0) {
                LED4_Write(1);
                turn_cooldown = 20000;
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
    //move_forward_ticks(50);   // Adjust 20–40 ticks depending on your map spacing

    stop();
CyDelay(100);
}

*/


void move_forward_until_intersection(void)
{
    static uint32_t turn_cooldown = 0;  // stays across calls
    int stable = 0;

    while (stable < 1)
    {
        uint8 o3 = Output_3_Read();  // right wing
        uint8 o6 = Output_6_Read();  // left wing

        // decrement cooldown to prevent re-trigger
        if (turn_cooldown > 0)
        {
            turn_cooldown--;
            go_straight();
            continue;   // skip detection while cooling down
        }

        bool left_turn  = false;
        bool right_turn = false;

        if ((o6 == 0) && (o3 == 0)) {
            LED2_Write(1);
            LED4_Write(1);
            right_turn = true;       // T intersection
        } 
        else if (o6 == 0) {
            LED2_Write(1);
            left_turn = true;
        } 
        else if (o3 == 0) {
            LED4_Write(1);
            right_turn = true;
        }

        bool intersection = left_turn || right_turn;

        if (intersection) {
            stable++;
            turn_cooldown = 75000;   // 80000 works
        } else {
            stable = 0;
            LED2_Write(0);
            LED4_Write(0);
        }

        go_straight();
    }

    stop();
    CyDelay(100);
}

// ============================================================================
// Run Until Food
// ============================================================================
void run_for_food(uint8_t food_distance)
{
         //if (food_index < food_len)
                //{
    go_straigh_with_tick(83*food_distance);   // move forward to food location
    stop();
    CyDelay(100);                  // 1 second pickup delay
                //}
}

void move_forward_skip_one_intersection(void)
{
    // First intersection → skip
    move_forward_until_intersection();

    // move forward along the line for 60 encoder ticks (ignore wings)
    QuadDec_M1_SetCounter(0);
    QuadDec_M2_SetCounter(0);
     int32 L = 0;
    while (abs(L) < 60)
    {
         L = ENCODER_LEFT_SIGN  * QuadDec_M1_GetCounter();
        LED1_Write(1);
        // follow the line using only front sensors
        go_straight();

        
         
    }
    LED1_Write(0);
    stop();
    CyDelay(50);

    // second intersection → stop here
    move_forward_until_intersection();
}


// ============================================================================
// Execute instruction list
// ============================================================================
void execute_instruction(uint8_t instr,uint16_t *food_distances)
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

        case U_TURN:
            u_turn_enc();
            break;
        case FOOD:
                if (food_index < 5)
                {
                    run_for_food(food_distances[food_index]);
                    food_index++;
                }

            break;
   
        default:
            stop();
            break;
    }
}

// ============================================================================
// Execute full instruction sequence
// ============================================================================
void execute_path(uint8_t *instructions, uint8_t length, uint16_t *food_distances)
{
    for (uint8_t i = 0; i < length; i++)
    {
        uint8_t current = instructions[i];

        
        
        if (current == STRAIGHT && (i + 1 < length) && instructions[i + 1] == STRAIGHT)
        {
            move_forward_skip_one_intersection();
            i++; // skip the next STRAIGHT since it’s handled
        }
        else
        {
            execute_instruction(current, food_distances);
        }
       

        if (current == STOP) break;
    }
}




