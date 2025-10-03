/* ========================================
 * Fully working code: 
 * PWM      : 
 * Encoder  : 
 * ADC      :
 * USB      : port displays speed and position.
 * CMD: "PW xx"
 * Copyright Univ of Auckland, 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Univ of Auckland.
 *
 * ========================================
*/

#include <project.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "main.h"
#include "defines.h"     
#include "vars.h"
#include "isr_1.h"
#include "SENSORS_READ.h"
#include "MOVEMENT.h"


#define ENCODER_CPR 500
#define QUAD_MULT 4        
#define COUNTS_PER_REV (ENCODER_CPR * QUAD_MULT)

#define TIMER_BASE_HZ 100000UL

#define PWM_FWD  168
#define PWM_STOP 127

static volatile int32_t enc_last = 0;      
static volatile int32_t enc_pos  = 0;      
static volatile float   spd_cps  = 0.0f;   
static volatile float   spd_rps  = 0.0f;   
static volatile float   spd_rpm  = 0.0f;   

static volatile uint8_t flag_print = 0;

volatile uint8 front_left_flag = 0;
volatile uint8 front_right_flag = 0;
volatile uint8 mid_left_flag = 0;
volatile uint8 mid_right_flag = 0;
volatile uint8 left_wing_flag = 0;
volatile uint8 right_wing_flag = 0;

static volatile int16 left_wheel_val; // positive
static volatile int16 right_wheel_val;//negative
static volatile int16 wheel_sum; // when wheel_sum is postive,left wheel is faster. otherwise. right wheel is faster
uint8_t change;
uint8_t left_pwm = 176;
uint8_t right_pwm = 176;


float Kp = 0.5;
float Ki = 0.1;
float Kd = 0.05;

static float integral = 0;
static float prev_error = 0;

uint8_t timer_flag = 0;
edge_pack_t edges = {0,0,0,0, 0, 0};

MovementState current_move;

// static void print_telemetry(void);
void usbPutString(char *s);
void usbPutChar(char c);
void handle_usb();

/* Timer ISR */
CY_ISR(Timer_TS_ISR_Handler)
{
    timer_flag = 1;    
    }


// helpers
#define S_ACTIVE(pin_read) ((pin_read) == 0u)   // active-low -> 1 when line

// active-low: 0 = on line, 1 = off line
#define S_ACTIVE(pin_read) ((pin_read) == 0u)

/* ================= Main ================= */
int main(void)
{
    CyGlobalIntEnable;
    PWM_1_Start();
    PWM_2_Start();
    
    isr_1_StartEx(Timer_TS_ISR_Handler);   // hook first
    Timer_TS_Start();                      // then start
    
    front_left_Start();
    front_right_Start();
    mid_left_Start();
    mid_right_Start();
    
    QuadDec_M1_Start();
    QuadDec_M2_Start();
    QuadDec_M1_SetCounter(0);
    QuadDec_M2_SetCounter(0);

#ifdef USE_USB
   USBUART_Start(0, USBUART_5V_OPERATION);
#endif


    // enable isr for edge detection

for(;;) {
    current_move = GetMovement();
    move_handling();
    if(current_move == STRAIGHT && timer_flag )
    {

        int error = QuadDec_M1_GetCounter() + QuadDec_M2_GetCounter();

        // PID
        integral += error;
        float derivative = error - prev_error;
        float output = Kp * error + Ki * integral + Kd * derivative;
        
        if (integral > 500) integral = 500;
        if (integral < -500) integral = -500;

        // Adjust PWM values
        left_pwm  -= (int)output;
        right_pwm += (int)output;

        // Clamp
        if (left_pwm < PID_PWM_MIN) left_pwm = PID_PWM_MIN;
        if (left_pwm > PID_PWM_MAX) left_pwm = PID_PWM_MAX;
        if (right_pwm < PID_PWM_MIN) right_pwm = PID_PWM_MIN;
        if (right_pwm > PID_PWM_MAX) right_pwm = PID_PWM_MAX;

        // Reset counters
        QuadDec_M1_SetCounter(0);
        QuadDec_M2_SetCounter(0);

        PWM_1_WriteCompare(left_pwm);
        PWM_2_WriteCompare(right_pwm);

        prev_error = error;
        timer_flag = 0;       
    }
}

}
 


void usbPutString(char *s)
{
#ifdef USE_USB
    while (USBUART_CDCIsReady() == 0) {}
    s[63] = '\0';
    s[62] = '!';              
    USBUART_PutData((uint8*)s, (uint16)strlen(s));
#else
    (void)s;
#endif
}

void usbPutChar(char c)
{
#ifdef USE_USB
    while (USBUART_CDCIsReady() == 0) {}
    USBUART_PutChar((uint8)c);
#else
    (void)c;
#endif
}

void handle_usb(void)
{
#ifdef USE_USB
    static uint8 usbStarted = FALSE;
    static uint16 usbBufCount = 0;

    if (!usbStarted) {
        if (USBUART_GetConfiguration()) {
            USBUART_CDC_Init();
            usbStarted = TRUE;
        }
        return;
    }

    if (USBUART_DataIsReady() == 0) return;

    uint8 c = USBUART_GetChar();

    if ((c == CHAR_ENTER) || (c == '\n')) {
        entry[usbBufCount] = '\0';
        strcpy(line, entry);
        usbBufCount = 0;
        flag_KB_string = 1;
    } else {
        if (((c == CHAR_BACKSP) || (c == CHAR_DEL)) && (usbBufCount > 0)) {
            usbBufCount--;
        } else {
            if (usbBufCount > (BUF_SIZE - 2)) {
                USBUART_PutChar('!');
            } else {
                entry[usbBufCount++] = (char)c;
            }
        }
    }
#endif
}

