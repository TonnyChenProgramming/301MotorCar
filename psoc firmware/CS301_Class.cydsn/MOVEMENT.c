#include "project.h"
#include "MOVEMENT.h"
#include "SENSORS_READ.h"
#include "main.h"
#include "SENSORS_READ.h"
//#include "Front_Left_Led.h"
//#include "Front_Right_Led.h"
//#include "Mid_Left_Led.h"
//#include "Mid_Right_Led.h"
//#include "Mid_Left_Led.h"
//#include "Right_Wing_Led.h"
//#include "Left_Wing_Led.h"

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

float Kp = 0.78;
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
            motor_left(30);
            motor_right(168);

            while (Output_4_Read() == 1 && Output_5_Read() == 1) {
                m = LEFT_TURN;
            }
            
            do_straight_with_pid();
            
            break;

        case RIGHT_TURN:
            motor_left(168);
            motor_right(30);
            
            while (Output_4_Read() == 0 && Output_5_Read() == 0) {
                m = RIGHT_TURN;
            }
            
            do_straight_with_pid();
            
            break;

        default:
            do_straight_with_pid();
            break;
    }

    // --- 4. Update previous state ---
    previous_movement = m;
}
