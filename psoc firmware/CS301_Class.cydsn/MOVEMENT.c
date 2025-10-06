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
float Kp = 30.0;
float Ki = 0.0;
float Kd = 10.0;
static float integral = 0;
static float prev_error = 0;

void do_straight_with_pid(void)
{
    int error = GetLineError();  // -1, 0, +1 from middle sensors

    // PID calculations
    integral += error;
    if (integral > 50) integral = 50;
    if (integral < -50) integral = -50;
    float derivative = error - prev_error;
    float output = Kp * error + Ki * integral + Kd * derivative;

    int base_speed = 170;  // forward PWM
    int left_pwm  = base_speed - (int)output;
    int right_pwm = base_speed + (int)output;

    // clamp
    if (left_pwm < 120) left_pwm = 120;
    if (left_pwm > 200) left_pwm = 200;
    if (right_pwm < 120) right_pwm = 120;
    if (right_pwm > 200) right_pwm = 200;

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
            do_straight_with_pid();  // PID active for straight line
            break;

        case LEFT_TURN:
            motor_left(PWM_STOP);    // pivot left
            motor_right(PWM_FWD);
            break;

        case RIGHT_TURN:
            motor_left(PWM_FWD);
            motor_right(PWM_STOP);   // pivot right
            break;

        case WAIT:
            // Keep last turn command active or stop
            // simplest: repeat previous motor command
            switch (previous_movement) {
                case LEFT_TURN:
                    motor_left(PWM_STOP);
                    motor_right(PWM_FWD);
                    break;

                case RIGHT_TURN:
                    motor_left(PWM_FWD);
                    motor_right(PWM_STOP);
                    break;

                case STRAIGHT:
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

