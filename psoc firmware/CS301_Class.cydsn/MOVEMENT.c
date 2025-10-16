#include "project.h"
#include "MOVEMENT.h"
#include "SENSORS_READ.h"
#include "main.h"
#include "SENSORS_READ.h"

extern MovementState previous_movement;

static void motor_left(uint16 val)  { PWM_1_WriteCompare(val); }
static void motor_right(uint16 val) { PWM_2_WriteCompare(val); }

uint8_t left_pwm = 160; //170
uint8_t right_pwm = 162; //171

uint8_t drift_right_pwm = 169; //179
uint8_t drift_left_pwm = 169;  //179


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
            motor_left(80);
            motor_right(167);

            while ((Output_5_Read() == 1)) {
                m = LEFT_TURN;
            }
            
            motor_left(127);
            motor_right(127);
            go_straight();
            
            break;

        case RIGHT_TURN:
            //164,80
            
            motor_left(151); //170, 80
            motor_right(94);// 154, 90
            
            while (Output_4_Read() == 1) {
                m = RIGHT_TURN;
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
