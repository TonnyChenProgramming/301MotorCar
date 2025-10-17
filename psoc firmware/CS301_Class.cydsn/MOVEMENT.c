#include "project.h"
#include "MOVEMENT.h"
#include "SENSORS_READ.h"
#include "main.h"
#include "SENSORS_READ.h"

extern MovementState previous_movement;

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

    // --- Minimal intersection-based helpers for instruction execution ---

    // Returns true if robot is at an intersection (both front sensors or both wings see line)
    bool is_at_intersection(void)
    {
        return ((Output_4_Read() == 0) && (Output_5_Read() == 0)) ||
               ((Output_3_Read() == 0) && (Output_6_Read() == 0));
    }

    // Returns true if robot is on the line (either front sensor sees line)
    bool on_line(void)
    {
        return (Output_4_Read() == 0) || (Output_5_Read() == 0);
    }

    // Move forward, following the line, until an intersection is detected
    void move_forward_until_intersection(void)
    {
        while (!is_at_intersection()) {
            go_straight();
            CyDelay(10); // small delay to reduce busy loop
        }
        stop();
    }

    // At intersection, turn left until front sensors reacquire the line
    void turn_left_until_line(void)
    {
        // Use the same PWM values as LEFT_TURN case
        // Start the turn and keep turning until we detect the line again
        do {
            motor_left(96);
            motor_right(156);
            CyDelay(5);
        } while (!on_line());
        stop();
    }

    // At intersection, turn right until front sensors reacquire the line
    void turn_right_until_line(void)
    {
        // Use the same PWM values as RIGHT_TURN case
        do {
            motor_left(154);
            motor_right(94);
            CyDelay(5);
        } while (!on_line());
        stop();
    }
