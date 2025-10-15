#include "project.h"
#include "MOVEMENT.h"
#include "SENSORS_READ.h"
#include "main.h"
#include "SENSORS_READ.h"

extern volatile uint16 tick_count;
extern volatile uint8 delay_active;
extern volatile uint16 delay_target;

#define TICK_MS 5                        // one tick = 5 ms (if 200 Hz ISR)
#define TICKS_MS(ms)  ((ms) / TICK_MS)   // convert ms to ticks

static inline void start_delay(uint16 ms)
{
    delay_target = tick_count + TICKS_MS(ms);
    delay_active = 1;
}

static inline uint8 delay_elapsed(void)
{
    if (delay_active && (tick_count >= delay_target))
    {
        delay_active = 0;
        return 1;
    }
    return 0;
}


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
//uint8_t left_pwm = 163;
//uint8_t right_pwm = 168;
#define BASE_PWM_LEFT   163
#define BASE_PWM_RIGHT  168

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
    static MovementState last = STRAIGHT;
    static uint8 pause_mode = 0;   // 1 = pre-turn, 2 = post-turn
    MovementState m = GetMovement();

    // --- 1. Intersection rule (both wings black) ---
    if (Output_6_Read() == 0 && Output_3_Read() == 0) {
        m = RIGHT_TURN;   // always take right turn
    }

    // --- 2. Handle active delay ---
if (delay_active)
{
    motor_left(PWM_STOP);
    motor_right(PWM_STOP);

    if (delay_elapsed())
    {
        pause_mode = 0;
        previous_movement = STRAIGHT;   // ✅ reset after pause
    }

    return; // skip rest until pause ends
}


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
            delay_active = 0;
            pause_mode = 0;

            if ((last == STRAIGHT) && pause_mode == 0) {
                // pre-turn pause
                start_delay(300);
                pause_mode = 1;
                return;
            }

            motor_left(70);
            motor_right(168);

            if (Output_4_Read() == 0 && Output_5_Read() == 0) {
                // post-turn pause when front sensors reacquire
                motor_left(PWM_STOP);
                motor_right(PWM_STOP);
                start_delay(300);
                pause_mode = 2;
                m = STRAIGHT;
            }
            break;

        case RIGHT_TURN:
            
            delay_active = 0;
            pause_mode = 0;

            if ((last == STRAIGHT) && pause_mode == 0) {
                // pre-turn pause
                start_delay(300);
                pause_mode = 1;
                return;
            }

            motor_left(168);
            motor_right(70);

            if (Output_4_Read() == 0 && Output_5_Read() == 0) {
                // post-turn pause
                motor_left(PWM_STOP);
                motor_right(PWM_STOP);
                start_delay(300);
                pause_mode = 2;
                m = STRAIGHT;
            }
            break;

        default:
            do_straight_with_pid();
            break;
    }

    // --- 4. Update previous state ---
    last = m;
    previous_movement = m;
}
