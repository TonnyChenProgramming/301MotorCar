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
float Kp = 4.0;
float Ki = 0.0;
float Kd = 3.0;
static float integral = 0;
static float prev_error = 0;

void do_straight_with_pid(void)
{
    int error = GetLineError();  // -1, 0, +1 from middle sensors
    
    if (error == 0 && previous_movement == STOP) {
        motor_left(168);
        motor_right(168);
        return;
    }
    
    // PID calculations
    integral += error;
    if (integral > 50) integral = 50;
    if (integral < -50) integral = -50;
    float derivative = error - prev_error;
    float output = Kp * error + Ki * integral + Kd * derivative;
    
    output *= 0.1;

    int base_speed = 170;  // forward PWM
    int correction = (int)(output);
    
    int left_pwm  = base_speed - correction;
    int right_pwm = base_speed + correction;

    // clamp
    if (left_pwm < 150) left_pwm = 150;
    if (left_pwm > 190) left_pwm = 190;
    if (right_pwm < 150) right_pwm = 150;
    if (right_pwm > 190) right_pwm = 190;

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

