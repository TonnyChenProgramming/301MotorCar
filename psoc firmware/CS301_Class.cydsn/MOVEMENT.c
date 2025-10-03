#include "project.h"
#include "MOVEMENT.h"
#include "SENSORS_READ.h"

// PWM values
#define PWM_FWD  168   // minimum forward torque
#define PWM_BWD   86   // backward value
#define PWM_STOP   127

// Motor control helpers
static void motor_left(uint16 val)  { PWM_1_WriteCompare(val); }
static void motor_right(uint16 val) { PWM_2_WriteCompare(val); }

void stop(void)
{
    motor_left(PWM_STOP);
    motor_right(PWM_STOP);
}

// Follow line until STOP or intersection
void move_straight(void)
{
    for (;;)
    {
        MovementState m = GetMovement();

        if (m == STOP) {
            stop();
            break;
        }
        else if (m == STRAIGHT) {
            motor_left(PWM_FWD);
            motor_right(PWM_FWD);
        }
        else if (m == DRIFTED_LEFT) {
            // robot is too far right, steer left
            motor_left(PWM_FWD);
            motor_right(PWM_STOP);
        }
        else if (m == DRIFTED_RIGHT) {
            // robot is too far left, steer right
            motor_left(PWM_STOP);
            motor_right(PWM_FWD);
        }
        else if (m == LEFT_TURN || m == RIGHT_TURN) {
            // simple turn available: keep following line as straight
            motor_left(PWM_FWD);
            motor_right(PWM_FWD);
        }
    }
}

// Rotate
