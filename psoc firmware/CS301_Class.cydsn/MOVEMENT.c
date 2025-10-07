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
float Kp = 0.5;
float Ki = 0.1;
float Kd = 0.05;
static float integral = 0;
static float prev_error = 0;
int error;
float derivative;
float output;
uint8_t left_pwm = 176;
uint8_t right_pwm = 176;


void do_straight_with_pid(void)
{

        error = QuadDec_M1_GetCounter() + QuadDec_M2_GetCounter();

        // PID
        integral += error;
        derivative = error - prev_error;
        output = Kp * error + Ki * integral + Kd * derivative;
        
        if (integral > 500) integral = 500;
        if (integral < -500) integral = -500;

        // Adjust PWM values
        left_pwm  -= (int)output;
        right_pwm += (int)output;

        // Clamp
        if (left_pwm < PID_PWM_MIN) left_pwm = PID_PWM_MIN;
        if (left_pwm > PID_PWM_MAX) left_pwm = PID_PWM_MAX;
        if (right_pwm < PID_PWM_MIN) right_pwm = PID_PWM_MIN;
        if (right_pwm > PID_PWM_MAX) right_pwm = PID_PWM_MAX;

        // Reset counters
        QuadDec_M1_SetCounter(0);
        QuadDec_M2_SetCounter(0);

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

    switch (m) {
        case STOP:
            stop();
            break;

        case STRAIGHT:
        case DRIFTED_LEFT:
        case DRIFTED_RIGHT:
            do_straight_with_pid();  // PID active for straight line
            break;

        case LEFT_TURN:
            motor_left(99);    // pivot left
            motor_right(168);
            break;

        case RIGHT_TURN:
            motor_left(168);
            motor_right(99);   // pivot right
            break;

        case WAIT:
            // Keep last turn command active or stop
            // simplest: repeat previous motor command
            switch (previous_movement) {
                case LEFT_TURN:
                    motor_left(127);
                    motor_right(255);
                    break;

                case RIGHT_TURN:
                    motor_left(255);
                    motor_right(127);
                    break;

                case STRAIGHT:
                case DRIFTED_LEFT:
                case DRIFTED_RIGHT:
                    do_straight_with_pid();
                    break;

                case STOP:
                default:
                    stop();
                    break;
            }
            break;

        default:
            stop();
            break;
    }
}

