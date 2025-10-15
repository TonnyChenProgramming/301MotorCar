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
float Ki = 0.10;
float Kd = 0.06;
static float integral = 0;
static float prev_error = 0;
int error;
float derivative;
float output;
int bias = 0;
//uint8_t left_pwm = 172;
//uint8_t right_pwm = 176;
#define BASE_PWM_LEFT   165
#define BASE_PWM_RIGHT  168
#define PID_PWM_MIN 155
#define PID_PWM_MAX 175

void usbPutString(char *s);

void do_straight_with_pid(uint16_t o1, uint16_t o2, uint16_t o3,
                          uint16_t o4, uint16_t o5, uint16_t o6)
{
    // --- 0) On-line detector (with hysteresis) ---
    // 0..1000, 0=black. Tune thresholds to your surface.
    const uint16_t FRONT_ON  = 100;  // enter "on-line" when both fronts < 100
    const uint16_t FRONT_OFF = 180;  // exit  "on-line" when any front  > 180
    const uint16_t MID_ON    = 120;  // optional mid check to avoid false locks

    static uint8_t on_line = 0;

    if (on_line) {
        if (o4 > FRONT_OFF || o5 > FRONT_OFF) on_line = 0;             // leave lock
    } else {
        if (o4 < FRONT_ON && o5 < FRONT_ON && o1 < MID_ON && o2 < MID_ON) on_line = 1; // lock
    }

    if (on_line) {
        // Hold straight; gently bleed integral so we don't snap when we leave the line
        integral *= 0.95f;              // small decay toward 0
        prev_error = 0.0f;              // keep D quiet
        derivative = 0.0f;
        output = 0.0f;

        int left_pwm  = BASE_PWM_LEFT;
        int right_pwm = BASE_PWM_RIGHT;

        motor_left(left_pwm);
        motor_right(right_pwm);
        return;                         // nothing else to do this tick
    }

    // --- 1) Build signed error from DUTY readings (0..1000, 0=black) ---
    // Positive error -> steer RIGHT (speed up left / slow right)
    const int S_FRONT = 50;  // main term scale (front pair)
    const int S_MID   = 80;  // gentler mid assist

    int error = 0;
    error += ((int)o4 - (int)o5) / S_FRONT;  // front-right vs front-left (want both near 0)
    error += ((int)o1 - (int)o2) / S_MID;    // mid-left vs mid-right assist

    // Optional tiny deadband so micro noise near center doesn’t twitch:
    if (error > -1 && error < 1) error = 0;

    // --- 2) PID core (no bias) ---
    integral += error;
    if (integral > 500) integral = 500;
    if (integral < -500) integral = -500;

    static float d_filt = 0.0f;
    float d_raw = (float)(error - prev_error);
    d_filt += 0.5f * (d_raw - d_filt);   // derivative smoothing (α=0.5)
    derivative = d_filt;

    output = Kp * error + Ki * integral + Kd * derivative;

    // --- 3) Differential steering around base PWMs ---
    int left_adjust  =  (int)output;
    int right_adjust = -(int)output;

    int left_pwm  = BASE_PWM_LEFT  + left_adjust;
    int right_pwm = BASE_PWM_RIGHT + right_adjust;

    if (left_pwm  < PID_PWM_MIN) left_pwm  = PID_PWM_MIN;
    if (left_pwm  > PID_PWM_MAX) left_pwm  = PID_PWM_MAX;
    if (right_pwm < PID_PWM_MIN) right_pwm = PID_PWM_MIN;
    if (right_pwm > PID_PWM_MAX) right_pwm = PID_PWM_MAX;

    motor_left(left_pwm);
    motor_right(right_pwm);

    prev_error = (float)error;
}

void do_straight_with_pid2(void)
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
            error -= 10;
        }
        if (edges.mid_left_edge)
        {
            edges.mid_left_edge = 0;
            error -= 10;
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
            error += 10;
        }
        if (edges.mid_right_edge)
        {
            edges.mid_right_edge = 0;
            error += 10;
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
void do_straight_with_pid3(void)
{
    /* ===== Tuning for tighter tracking ===== */
    Kp = 0.78f;    // was 0.65
    Ki = 0.10f;    // was 0.08
    Kd = 0.06f;    // was 0.05

    const float STEER = 0.28f;     // stronger steering than 0.20
    const int   EDGE_STEP = 12;    // sensor event magnitude (was 10)
    const float HEAD_GAIN = 0.08f; // encoder heading assist (small)

    /* Optionally widen rails a bit for more authority */
    #undef  PID_PWM_MIN
    #undef  PID_PWM_MAX
    #define PID_PWM_MIN 150
    #define PID_PWM_MAX 182

    /* ===== SENSE: build error from sensors + heading ===== */
    int  left_enc  = QuadDec_M1_GetCounter();
    int  right_enc = QuadDec_M2_GetCounter();

    int error_local = 0;

    // Photosensors: active-low on black
    const int left_on_black  = (Output_5_Read() == 0);
    const int right_on_black = (Output_4_Read() == 0);

    if (left_on_black  && !right_on_black) error_local += +EDGE_STEP;
    if (!left_on_black && right_on_black)  error_local += -EDGE_STEP;

    // Edge events (keep your semantics, but bump step for tighter response)
    if (left_on_black) {
        if (edges.front_right_edge) { edges.front_right_edge = 0; error_local -= EDGE_STEP; }
        if (edges.mid_left_edge)    { edges.mid_left_edge    = 0; error_local -= EDGE_STEP; }
    }
    if (right_on_black) {
        if (edges.front_left_edge)  { edges.front_left_edge  = 0; error_local += EDGE_STEP; }
        if (edges.mid_right_edge)   { edges.mid_right_edge   = 0; error_local += EDGE_STEP; }
    }

    // Small bias nudge only when sensors disagree (and error small)
    if (error_local > -6 && error_local < 6) {
        if ( left_on_black && !right_on_black) bias = +1;
        else if (!left_on_black &&  right_on_black) bias = -1;
        else bias = 0;
    } else {
        bias = 0;
    }

    // Heading assist from encoders: use LEFT - RIGHT (sign chosen so + means steer right)
    float e_head = (float)(left_enc - right_enc) * HEAD_GAIN;
    float error_f = (float)error_local + e_head;

    /* ===== PID with derivative smoothing + anti-windup ===== */
    static float d_filt = 0.0f;         // derivative low-pass (IIR)
    static uint8_t sat_prev = 0;        // previous saturation flag

    // Derivative on error, filtered (0.2..0.7; higher = snappier)
    const float D_ALPHA = 0.55f;
    float d_raw   = error_f - prev_error;
    d_filt       += D_ALPHA * (d_raw - d_filt);
    derivative    = d_filt;

    // Proportional now
    float p_term = Kp * error_f;
    float d_term = Kd * derivative;

    // Predict output without I to decide whether to integrate
    float u_no_i = p_term + d_term + (float)bias;
    int   l_test = BASE_PWM_LEFT  + (int)(STEER * u_no_i);
    int   r_test = BASE_PWM_RIGHT - (int)(STEER * u_no_i);
    uint8_t would_saturate =
        (l_test < PID_PWM_MIN) || (l_test > PID_PWM_MAX) ||
        (r_test < PID_PWM_MIN) || (r_test > PID_PWM_MAX);

    // Conditional integration: avoid winding up "against" the rails
    if (!would_saturate || !sat_prev) {
        integral += error_f;
        if (integral > 500.0f) integral = 500.0f;
        if (integral < -500.0f) integral = -500.0f;
    }

    // Final control
    output = (p_term + Ki * integral + d_term + (float)bias) * STEER;

    /* ===== Map to motors and clamp ===== */
    int left_adjust  =  (int)output;
    int right_adjust = -(int)output;

    int left_pwm  = BASE_PWM_LEFT  + left_adjust;
    int right_pwm = BASE_PWM_RIGHT + right_adjust;

    if (left_pwm  < PID_PWM_MIN) left_pwm  = PID_PWM_MIN;
    if (left_pwm  > PID_PWM_MAX) left_pwm  = PID_PWM_MAX;
    if (right_pwm < PID_PWM_MIN) right_pwm = PID_PWM_MIN;
    if (right_pwm > PID_PWM_MAX) right_pwm = PID_PWM_MAX;

    sat_prev = (left_pwm == PID_PWM_MIN) || (left_pwm == PID_PWM_MAX) ||
               (right_pwm == PID_PWM_MIN) || (right_pwm == PID_PWM_MAX);

    /* ===== Apply + housekeeping ===== */
    QuadDec_M1_SetCounter(0);
    QuadDec_M2_SetCounter(0);

    motor_left(left_pwm);
    motor_right(right_pwm);

    prev_error = error_f;
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
            do_straight_with_pid2();  // PID active for straight line
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