#include "project.h"
#include "MOVEMENT.h"
#include "SENSORS_READ.h"
#include "main.h"
#include "SENSORS_READ.h"
extern MovementState previous_movement;

// PWM values
#define PWM_FWD  168   // minimum forward torque
#define PWM_BWD   86   // backward value
#define PWM_STOP   127

// Motor control helpers
static void motor_left(uint16 val)  { PWM_1_WriteCompare(val); }
static void motor_right(uint16 val) { PWM_2_WriteCompare(val); }

// PID constants (tune these)
float Kp = 0.8f;
float Ki = 0.0f;
float Kd = 0.15f;
static float integral = 0;
static float prev_error = 0;
int error;
float derivative;
float output;


void usbPutString(char *s);

void do_straight_with_pid(void)
{
    
        int error = 0;
        
    // Left sensor (Output_5): if off, we drifted left → need to turn right (positive error)
    if (Output_5_Read()) error += 1;

    // Right sensor (Output_4): if off, we drifted right → need to turn left (negative error)
    if (Output_4_Read()) error -= 1;
   

        // PID
        integral += error;
        derivative = error - prev_error;
    
        output = (Kp * error) + (Ki * integral) + (Kd * derivative);
        output *= 0.1;
        
        //if (integral > 400) integral = 400;
        //if (integral < -400) integral = -400;
        
        if (output > 5) output = 5;
        if (output < -5) output = 5;
        
        
        int base_left = 165;
        int base_right = 165;
        
        int left_pwm  = base_left - (int)output;
        int right_pwm = base_right + (int)output;

        // Clamp
if (left_pwm < 150) left_pwm = 160;
if (left_pwm > 180) left_pwm = 170;
if (right_pwm < 150) right_pwm = 160;
if (right_pwm > 180) right_pwm = 170;
        
    motor_left(left_pwm);
    motor_right(right_pwm);

    prev_error = error;
}


void stop(void)
{
    motor_left(PWM_STOP);
    motor_right(PWM_STOP);
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
            motor_left(PWM_STOP);
            motor_right(PWM_STOP);
            break;

        case STRAIGHT:
            do_straight_with_pid();  // keep moving straight
            break;

        case LEFT_TURN:
            motor_left(50);
            motor_right(168);

            if (Output_4_Read() == 0 && Output_5_Read() == 0) {
                m = STRAIGHT;
            }
            break;

        case RIGHT_TURN:
            motor_left(168);
            motor_right(50);
            
            if (Output_4_Read() == 0 && Output_5_Read() == 0) {
                m = STRAIGHT;
            }
            break;

        default:
            do_straight_with_pid();
            break;
    }

    // --- 4. Update previous state ---
    previous_movement = m;
}
