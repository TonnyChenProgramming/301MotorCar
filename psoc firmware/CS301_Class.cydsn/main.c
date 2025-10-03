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
#include "LEFT_TURN.h"
#include "RIGHT_TURN.h"
#include "STRAIGHT.h"
#include "DRIFTED_RIGHT.h"
#include "DRIFTED_LEFT.h"
#include "SENSORS_READ.h"
#include "STOP.h"
#include "MOVEMENT.h"
#include "manoeuvre.h"

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


edge_pack_t edges = {0,0,0,0, 0, 0};

// static void print_telemetry(void);
void usbPutString(char *s);
void usbPutChar(char c);
void handle_usb();

/* Timer ISR */
CY_ISR(Timer_TS_ISR_Handler)
{
    // tick bookkeeping
    if (++ts_speed   >= DECIMATE_TS_SPEED)   { ts_speed = 0;   flag_ts_speed = 1;   }
    if (++ts_display >= DECIMATE_TS_DISPLAY) { ts_display = 0; flag_ts_display = 1; }

    // Do speed math only when due
    if (flag_ts_speed) {
        flag_ts_speed = 0;

        int32_t now   = QuadDec_M1_GetCounter();
        int32_t delta = now - enc_last;
        enc_last      = now;
        enc_pos       = now;

        // counts/sec over Δt = DECIMATE_TS_SPEED / TIMER_BASE_HZ
        // cps = delta * TIMER_BASE_HZ / DECIMATE_TS_SPEED
        float cps = (float)delta * ((float)TIMER_BASE_HZ / (float)DECIMATE_TS_SPEED);
        spd_cps = cps;

        float rev_per_sec = spd_cps / (float)(/* CPR × 4 */ 3 * 4 * 19); // = 228 if PDF motor (3 CPR, 19:1, x4)
        spd_rps = rev_per_sec * 2.0f * 3.14159265358979323846f;
        spd_rpm = rev_per_sec * 60.0f;
    }

#ifdef Timer_TS_ClearInterrupt
    Timer_TS_ClearInterrupt(Timer_TS_INTR_MASK_TC);
#else
    (void)Timer_TS_ReadStatusRegister();
#endif
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
    
    QuadDec_M1_Start();
    QuadDec_M1_SetCounter(0);
    enc_last = QuadDec_M1_GetCounter();
    
   front_left_Start();   /* uses front_left_Interrupt */



#ifdef USE_USB
   USBUART_Start(0, USBUART_5V_OPERATION);
#endif

    isr_1_StartEx(Timer_TS_ISR_Handler);   // hook first
    Timer_TS_Start();                      // then start
    
    // enable isr for edge detection


  #define ON_LINE(x) ((x)==0u)
for(;;) {
    CyDelay(100);
    LED3_Write(ON_LINE(Output_1_Read()));
    LED5_Write(ON_LINE(Output_2_Read()));
    LED4_Write(ON_LINE(Output_3_Read()));
    LED6_Write(ON_LINE(Output_4_Read()));
    LED1_Write(ON_LINE(Output_5_Read()));
    LED2_Write(ON_LINE(Output_6_Read()));
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

