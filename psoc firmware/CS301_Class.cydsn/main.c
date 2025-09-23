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

        float rev_per_sec = spd_cps / (float)(/* CPR × 4 */ 3 * 4 * 19); // = 228 if your PDF motor (3 CPR, 19:1, x4)
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
    RIGHT_TURN();
    QuadDec_M1_Start();
    QuadDec_M1_SetCounter(0);
    enc_last = QuadDec_M1_GetCounter();

#ifdef USE_USB
    USBUART_Start(0, USBUART_5V_OPERATION);
#endif

    isr_1_StartEx(Timer_TS_ISR_Handler);   // hook first
    Timer_TS_Start();                      // then start

    for(;;) {
        if (flag_ts_display) {
            flag_ts_display = 0;

            // if float printf isn’t enabled, convert to fixed point (see note below)
            char buf[64];
            snprintf(buf, sizeof(buf),
                     "pos:%ld cps:%.0f rpm:%.1f rps:%.2f\r\n",
                     (long)enc_pos, (double)spd_cps, (double)spd_rpm, (double)spd_rps);
            usbPutString(buf);
        }
        handle_usb();
        if (flag_KB_string) { usbPutString(line); flag_KB_string = 0; }
    }
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

