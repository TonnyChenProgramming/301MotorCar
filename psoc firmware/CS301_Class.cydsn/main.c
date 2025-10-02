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

#define ENCODER_CPR 3
#define QUAD_MULT 4    
#define GEAR_RATIO 19
#define CPR_WHEEL  (ENCODER_CPR * QUAD_MULT * GEAR_RATIO)
#define WHEEL_DIAMETER_M  0.065f
#define WHEEL_CIRC_M (3.14159265358979323846f * WHEEL_DIAMETER_M)

#define TIMER_BASE_HZ 10000UL

static volatile int32_t enc1_last = 0, enc2_last = 0;
static volatile int32_t enc1_pos  = 0, enc2_pos  = 0;
static volatile float   spd1_cps  = 0.0f, spd2_cps = 0.0f; 
static volatile float   spd1_mps = 0.0f, spd2_mps = 0.0f;

static volatile uint8_t flag_print = 0;

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

        // --- Left wheel ---
        int32_t now1   = QuadDec_M1_GetCounter();
        int32_t delta1 = now1 - enc1_last;
        enc1_last      = now1;
        enc1_pos       = now1;

        // --- Right wheel ---
        int32_t now2   = QuadDec_M2_GetCounter();
        int32_t delta2 = now2 - enc2_last;
        enc2_last      = now2;
        enc2_pos       = now2;

        // counts/sec over Δt = DECIMATE_TS_SPEED / TIMER_BASE_HZ
        const float cps_scale = (float)TIMER_BASE_HZ / (float)DECIMATE_TS_SPEED;
        spd1_cps = (float)delta1 * cps_scale;
        spd2_cps = (float)delta2 * cps_scale;
        float revps1 = spd1_cps / (float)CPR_WHEEL, revps2 = spd2_cps / (float)CPR_WHEEL;
        spd1_mps = revps1 * WHEEL_CIRC_M;
        spd2_mps = revps2 * WHEEL_CIRC_M;
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
    QuadDec_M2_Start();
    QuadDec_M1_SetCounter(0);
    QuadDec_M2_SetCounter(0);
    enc1_last = QuadDec_M1_GetCounter();
    enc2_last = QuadDec_M2_GetCounter();

#ifdef USE_USB
    USBUART_Start(0, USBUART_5V_OPERATION);
#endif

    isr_1_StartEx(Timer_TS_ISR_Handler);   // hook first
    Timer_TS_Start();                      // then start
const int32_t target_pos = 1241;  // set your desired count
    for(;;) {
      //MOVE_STRAIGHT();
      //if (Output_6_Read() == 0){ TURN_LEFT();}
      //if (Output_3_Read() == 0) {TURN_RIGHT();}
      //move(GetMovement());
        

        if (abs(QuadDec_M2_GetCounter()) >= target_pos) {
            while(1){
            PWM_1_WriteCompare(127);
            PWM_2_WriteCompare(127);}
            enc2_pos = 0;
        }
        else {
            PWM_1_WriteCompare(204);
            PWM_2_WriteCompare(209);
        }
        if (flag_ts_display) {
            flag_ts_display = 0;
            
            char buf[120];
            snprintf(buf, sizeof(buf),
                     "Lpos:%ld Rpos:%ld  Lcps:%.0f Rcps:%.0f  L:%.3f m/s R:%.3f m/s\r\n",
                     (long)enc1_pos, (long)enc2_pos,
                     (double)spd1_cps, (double)spd2_cps,
                     (double)spd1_mps, (double)spd2_mps);
            usbPutString(buf);
            
          }
    

         // MovementState move = GetMovement();
        //  switch(move) {
        //    case STRAIGHT:          MOVE_STRAIGHT();    break;
        //    case LEFT_TURN:         TURN_LEFT();        break;
          //  case RIGHT_TURN:        TURN_RIGHT();       break;
           // case DRIFTED_LEFT:      DRIFT_RIGHT();      break;
            //case DRIFTED_RIGHT:     DRIFT_LEFT();       break;
            //case STOP:              STOP_MOVING();      break;
        
        
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
}
void usbPutString(char *s)
{
#ifdef USE_USB
    if (USBUART_CDCIsReady()) {
        USBUART_PutData((uint8*)s, (uint16)strlen(s));
    }
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

