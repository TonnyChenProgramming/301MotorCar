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

#define TIMER_BASE_HZ 10000UL

static volatile int32_t enc_last = 0;      
static volatile int32_t enc_pos  = 0;      
static volatile float   spd_cps  = 0.0f;   
static volatile float   spd_rps  = 0.0f;   
static volatile float   spd_rpm  = 0.0f;   

static volatile uint8_t flag_print = 0;

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


/* ================= Main ================= */
int main(void)
{
    CyGlobalIntEnable;
    PWM_1_Start();
    PWM_2_Start();
    
    QuadDec_M1_Start();
    QuadDec_M1_SetCounter(0);
    enc_last = QuadDec_M1_GetCounter();

#ifdef USE_USB
    USBUART_Start(0, USBUART_5V_OPERATION);
#endif

    isr_1_StartEx(Timer_TS_ISR_Handler);   // hook first
    Timer_TS_Start();                      // then start
    //MOVE_STRAIGHT();
    // enable isr for edge detection
    front_left_Start();
    front_right_Start();
    mid_left_Start();
    mid_right_Start();
    left_wing_Start();
    right_wing_Start();
    for(;;) {
      //MOVE_STRAIGHT();
      //if (Output_6_Read() == 0){ TURN_LEFT();}
      //if (Output_3_Read() == 0) {TURN_RIGHT();}
      MovementState movement = GetMovement();
      move(movement);
    if (movement)
    {
      if (edges.front_left_edge)
    {
        edges.front_left_edge = 0;
        edge_front_left_manoeuvre();
        
    } 
    else if (edges.front_right_edge)
    {
        edges.front_right_edge = 0;
        edge_front_right_manoeuvre();
    } 
    else if (edges.mid_left_edge)
    {
        edges.mid_left_edge = 0;
        edge_mid_left_manoeuvre();        
    } 
    else if (edges.mid_right_edge)
    {
        edges.mid_right_edge = 0;
        edge_mid_right_manoeuvre();        
    }
    
        else if (edges.left_wing_edge)
    {
        edges.left_wing_edge = 0;
        edge_left_wing_manoeuvre();
    }
    else if (edges.right_wing_edge)
    {
        edges.right_wing_edge = 0;
        edge_right_wing_manoeuvre();
    }
   }

    /*
        if (flag_ts_display) {
            flag_ts_display = 0;

            // if float printf isn’t enabled, convert to fixed point (see note below)
            char buf[64];
            
            snprintf(buf, sizeof(buf),
                     "pos:%ld cps:%.0f rpm:%.1f rps:%.2f\r\n",
                     (long)enc_pos, (double)spd_cps, (double)spd_rpm, (double)spd_rps);
            usbPutString(buf);
            
    */

          }
    
/*
          MovementState move = GetMovement();
          switch(move) {
            case STRAIGHT:          MOVE_STRAIGHT();    break;
            case LEFT_TURN:         TURN_LEFT();        break;
            case RIGHT_TURN:        TURN_RIGHT();       break;
            case DRIFTED_LEFT:      DRIFT_RIGHT();      break;
            case DRIFTED_RIGHT:     DRIFT_LEFT();       break;
            case STOP:              STOP_MOVING();      break;
        }
*/       
        
        handle_usb();
        if (flag_KB_string) { usbPutString(line); flag_KB_string = 0; }
    }
    
/* static void print_telemetry(void)
{
    uint8 intr = CyEnterCriticalSection();
    int32_t pos = enc_pos;
    float cps   = spd_cps;
    float rpm   = spd_rpm;
    float rps   = spd_rps;
    CyExitCriticalSection(intr);

    char buf[64];
    snprintf(buf, sizeof(buf), "pos:%ld cps:%.0f rpm:%.1f rps:%.2f\r\n",
   (long)pos, (double)cps, (double)rpm, (double)rps);
    usbPutString(buf);
}
*/
//}
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

