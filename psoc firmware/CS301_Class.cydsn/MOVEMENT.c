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
//uint8_t left_pwm = 172;
//uint8_t right_pwm = 176;
#define BASE_PWM_LEFT   172
#define BASE_PWM_RIGHT  176

void usbPutString(char *s);

void do_straight_with_pid(void)
{
    
        int error = 0;
        
        //int left_enc = QuadDec_M1_GetCounter();
       // int right_enc = QuadDec_M2_GetCounter();
        // Left sensor (Output_5): if off, we drifted left → need to turn right (positive error)
    if (!Output_5_Read()) error += 1;

    // Right sensor (Output_4): if off, we drifted right → need to turn left (negative error)
    if (!Output_4_Read()) error -= 1;
   

        // PID
        integral += error;
        derivative = error - prev_error;
        output = Kp * error + Ki * integral + Kd * derivative;
        output *= 0.2;
        
        if (integral > 500) integral = 500;
        if (integral < -500) integral = -500;

        int left_adjust  = (int)output;
        int right_adjust = -(int)output;

        int left_pwm  = BASE_PWM_LEFT  + left_adjust;
        int right_pwm = BASE_PWM_RIGHT + right_adjust;

        // Clamp
        if (left_pwm < PID_PWM_MIN) left_pwm = PID_PWM_MIN;
        if (left_pwm > PID_PWM_MAX) left_pwm = PID_PWM_MAX;
        if (right_pwm < PID_PWM_MIN) right_pwm = PID_PWM_MIN;
        if (right_pwm > PID_PWM_MAX) right_pwm = PID_PWM_MAX;

        // Reset counters
      //  QuadDec_M1_SetCounter(0);
      //  QuadDec_M2_SetCounter(0);

    motor_left(left_pwm);
    motor_right(right_pwm);

    prev_error = error;
    
    //debug
    ///#ifdef USE_USB
    //char buf[64];
    //sprintf(buf, "L:%d R:%d Err:%d Int:%d Der:%d Out:%d LPW:%d RPMW:%d\r\n",
     //       left_enc, right_enc, error, (int)integral, (int)derivative, (int)output, left_pwm, right_pwm);
    //usbPutString(buf);
    //#endif
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
            motor_left(127);    // pivot left
            motor_right(127);
            break;

        case RIGHT_TURN:
            motor_left(168);
            motor_right(99);   // pivot right
            break;

   /*     case WAIT:
           // Keep turning in the same direction until middle sensors see line again
    if (Output_5_Read() && Output_4_Read()) {
        // Line reacquired (assuming active-low sensors: 0 = line detected)
        do_straight_with_pid();
        previous_movement = STRAIGHT;
    } 
    else {
        // Continue the previous turn direction
        if (previous_movement == LEFT_TURN) {
            motor_left(127);
            motor_right(168);   // keep turning left
        } 
        else if (previous_movement == RIGHT_TURN) {
            motor_left(168);
            motor_right(99);    // keep turning right
        } 
        else {
            // Default to stop if no previous movement known
            stop();
        }
    }
    break;
*/
        default:
            stop();
            break;
         
        }
        if (m != WAIT) {
            previous_movement = m;
    }
}
