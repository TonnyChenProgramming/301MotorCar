#include "project.h"
#include <stdbool.h>
#include "MOVEMENT.h"
#include "SENSORS_READ.h"
#include "main.h"
#include "SENSORS_READ.h"

extern MovementState previous_movement;
extern uint8_t timer_flag; // from main.c Timer ISR

static void motor_left(uint16 val)  { PWM_1_WriteCompare(val); }
static void motor_right(uint16 val) { PWM_2_WriteCompare(val); }
#define counter_left 20000
#define counter_right 20000
uint8_t left_pwm = 154; //163
uint8_t right_pwm = 157; //165 

uint8_t drift_left_pwm = 162;  //169
uint8_t drift_right_pwm = 158; //165

uint16_t count_down = 0;
void usbPutString(char *s);

void go_straight(void)
{
    // Left sensor not in line (drift right)
    if ((Output_5_Read()) && !(Output_4_Read())) {
        motor_left(drift_right_pwm);
        motor_right(right_pwm);
    }
    
    // Right sensor not in line (drift left)
    else if (!(Output_5_Read()) && (Output_4_Read())) {
        motor_left(left_pwm);
        motor_right(drift_left_pwm);
    }
    
    else {
        motor_left(left_pwm);
        motor_right(right_pwm);
    }

}



void stop(void)
{
    motor_left(127);
    motor_right(127);
}

void move_handling(void)
{
    MovementState m = GetMovement();

    // --- 1. Intersection rule (both wings black) ---
  //  if (Output_6_Read() == 0 && Output_3_Read() == 0) {
    //    m = RIGHT_TURN;   // always take right turn
   // }

    // --- 3. Execute main movement ---
    switch (m)
    {
        case STOP:
            motor_left(127);
            motor_right(127);
            break;

        case STRAIGHT:
            go_straight();  // keep moving straight
            break;

        case LEFT_TURN:
            // 54, 195
            //80, 170
            motor_left(96);
            motor_right(156);

            while ((Output_4_Read() == 1) && (Output_5_Read() == 1)) {
                m = LEFT_TURN;
                count_down = counter_left;
            }
            while (count_down > 0)
            {
                count_down--;
            }
            
            motor_left(127);
            motor_right(127);
            go_straight();
            
            break;

        case RIGHT_TURN:
            //164,80
            
            motor_left(154); //170, 80
            motor_right(94);// 154, 90
            
            while ((Output_5_Read() == 1) && (Output_4_Read() == 1)) {
                m = RIGHT_TURN;
                count_down = counter_right;
            }
            while (count_down > 0)
            {
                count_down--;
            }
            
            motor_left(127);
            motor_right(127);
            go_straight();
            
            break;

        default:
            go_straight();
            break;
    }

    // --- 4. Update previous state ---
    previous_movement = m;
}

// --- Junction and line helpers ---

// Compute availability of branches based on active-low sensors
static inline void read_junction_options(bool *hasStraight, bool *hasLeft, bool *hasRight)
{
    uint8 o3 = Output_3_Read(); // right wing (0=line)
    uint8 o6 = Output_6_Read(); // left wing  (0=line)
    uint8 o4 = Output_4_Read(); // front-right (0=line)
    uint8 o5 = Output_5_Read(); // front-left  (0=line)
    if (hasLeft)    *hasLeft    = (o6 == 0);
    if (hasRight)   *hasRight   = (o3 == 0);
    if (hasStraight)*hasStraight= (o4 == 0) || (o5 == 0);
}

// Returns true if robot is at a decision point (any branch or dead-end)
bool is_at_intersection(void)
{
    bool s=false,l=false,r=false;
    uint8 o4 = Output_4_Read();
    uint8 o5 = Output_5_Read();
    read_junction_options(&s,&l,&r);
    bool bothFront = (o4 == 0) && (o5 == 0);
    bool deadEnd = !s; // straight not available
    return l || r || bothFront || deadEnd;
}

// Returns true if robot is on (any) line with front sensors
bool on_line(void)
{
    return (Output_4_Read() == 0) || (Output_5_Read() == 0);
}

// Move forward, following the line, until any decision point is detected
void move_forward_until_intersection(void)
{
    int stable = 0;
    while (stable < 3) {
        if (timer_flag) {
            timer_flag = 0;
            if (is_at_intersection()) stable++; else stable = 0;
        }
        go_straight();
    }
    stop();
}

// At a decision point, turn left until front sensors reacquire the line
void turn_left_until_line(void)
{
    int stable = 0;
    while (stable < 3) {
        if (timer_flag) {
            timer_flag = 0;
            if (on_line()) stable++; else stable = 0;
        }
        motor_left(96);
        motor_right(156);
    }
    stop();
}

// At a decision point, turn right until front sensors reacquire the line
void turn_right_until_line(void)
{
    int stable = 0;
    while (stable < 3) {
        if (timer_flag) {
            timer_flag = 0;
            if (on_line()) stable++; else stable = 0;
        }
        motor_left(154);
        motor_right(94);
    }
    stop();
}

// Move forward, following the line, until a left turn becomes available (with or without straight)
void move_until_left_turn(void)
{
    int stable = 0;
    while (stable < 3) {
        if (timer_flag) {
            timer_flag = 0;
            bool s=false,l=false,r=false;
            read_junction_options(&s,&l,&r);
            if (l) stable++; else stable = 0;
        }
        go_straight();
    }
    stop();
}

// Move forward, following the line, until a right turn becomes available (with or without straight)
void move_until_right_turn(void)
{
    int stable = 0;
    while (stable < 3) {
        if (timer_flag) {
            timer_flag = 0;
            bool s=false,l=false,r=false;
            read_junction_options(&s,&l,&r);
            if (r) stable++; else stable = 0;
        }
        go_straight();
    }
    stop();
}
