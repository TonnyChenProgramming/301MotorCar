#include "project.h"
#include "MOVEMENT.h"
#include "SENSORS_READ.h"
#include "main.h"
#include "SENSORS_READ.h"
#include "Front_Left_Led.h"
#include "Front_Right_Led.h"
#include "Mid_Left_Led.h"
#include "Mid_Right_Led.h"
#include "Mid_Left_Led.h"
#include "Right_Wing_Led.h"
#include "Left_Wing_Led.h"

extern MovementState previous_movement;

// PWM values
#define PWM_FWD  168   // minimum forward torque
#define PWM_BWD   86   // backward value
#define PWM_STOP   127
//Led display for Debugging
static void Led_Turn_Off(void);
// Motor control helpers
static void motor_left(uint16 val)  { PWM_1_WriteCompare(val); }
static void motor_right(uint16 val) { PWM_2_WriteCompare(val); }

// PID constants (tune these)
//float Kp = 0.5;
//float Ki = 0.1;
//float Kd = 0.05;
float Kp = 0.78;
//float Kp = 0.65;
float Ki = 0.10;
float Kd = 0.06;
static float integral = 0;
static float prev_error = 0;
int error;
float derivative;
float output;
int bias = 0;
uint8_t left_pwm = 175;
uint8_t right_pwm = 170;
//#define BASE_PWM_LEFT   200
//#define BASE_PWM_RIGHT  195
//#define PID_PWM_MIN 195
//#define PID_PWM_MAX 205  
//20 error
#define BASE_PWM_LEFT   175
#define BASE_PWM_RIGHT  170
#define PID_PWM_MIN 170
#define PID_PWM_MAX 180
#define CHANGE_IN_ERROR 120

void usbPutString(char *s);




void do_straight_with_pid(void)
{
    
        int error = 0;
        
        //int left_enc = QuadDec_M1_GetCounter();
       // int right_enc = QuadDec_M2_GetCounter();
    //error = QuadDec_M1_GetCounter() + QuadDec_M2_GetCounter();
        // Left sensor (Output_5): if off, we drifted left → need to turn right (positive error)
    //if (Output_5_Read()) error += 5;
    //if ((!Output_4_Read()) && (!Output_5_Read()) && (edges.front_left_edge|edges.front_right_edge|edges.mid_right_edge|edges.mid_left_edge)){
        //both sensor on black line 
       // error = QuadDec_M1_GetCounter() + QuadDec_M2_GetCounter();
        //error = 0.4 * error;
    //} else 
    if (!Output_5_Read()){
        // Right sensor (Output_4): if off, we drifted right → need to turn left (negative error)
        if (edges.front_right_edge)
        {
            edges.front_right_edge = 0;
            error -= CHANGE_IN_ERROR;
        }
        if (edges.mid_left_edge)
        {
            edges.mid_left_edge = 0;
            error -= CHANGE_IN_ERROR;
        }
        if (Output_4_Read())
        {
            bias = -1;
        } else{
            bias = 0;
        } }else if (!Output_4_Read()){
        if (edges.front_left_edge)
        {
            edges.front_left_edge = 0;
            error += CHANGE_IN_ERROR;
        }
        if (edges.mid_right_edge)
        {
            edges.mid_right_edge = 0;
            error += CHANGE_IN_ERROR;
        }
        if (Output_5_Read())
        {
            bias = 1;
        } else{
            bias = 0;
        }       
    
    }


    
    

        // PID
        integral += error;
     // NEW: light smoothing on D for tighter control without jitter (1 line + 1 static var)
        static float d_filt = 0.0f;
        float d_raw = (float)(error - prev_error);
        d_filt += 0.5f * (d_raw - d_filt);     // alpha 0.5; lower = smoother, higher = snappier
        derivative = d_filt;
        output = Kp * error + Ki * integral + Kd * derivative + bias;
        //output *= 0.2;
        
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

void move_handling(MovementState m)
{
    Led_Turn_Off();
    switch (m) {
        case STOP:
            stop();
            break;

        case STRAIGHT:
            QuadDec_M1_SetCounter(0);
            QuadDec_M2_SetCounter(0);
            Front_Left_Led_Write(1);
            Front_Right_Led_Write(1);
            do_straight_with_pid();  // PID active for straight line
            break;

        case LEFT_TURN:
            Left_Wing_Led_Write(1);
            motor_left(90);    // pivot left
            motor_right(168);
            break;

        case RIGHT_TURN:
            Right_Wing_Led_Write(1);
            motor_left(168);
            motor_right(90);   // pivot right
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
void Led_Turn_Off(void)
{
    Front_Left_Led_Write(0);
    Front_Right_Led_Write(0);
    Mid_Left_Led_Write(0);
    Mid_Right_Led_Write(0);
    Left_Wing_Led_Write(0);
    Right_Wing_Led_Write(0);
}