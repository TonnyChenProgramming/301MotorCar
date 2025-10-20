// ========================================
// Main File – Debug Removed (No Putty Output)
// ========================================

#include <project.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "main.h"
#include "defines.h"     
#include "vars.h"
#include "isr_1.h"
#include "SENSORS_READ.h"
#include "MOVEMENT.h"
#include "map_to_instructions.h" 

#define TIMER_BASE_HZ 100000UL
//#define USE_USB 1

// --- Globals ---
static volatile int16 left_wheel_val;
static volatile int16 right_wheel_val;
uint8_t timer_flag = 0;
uint8_t food_index = 0;
MovementState current_move;

// --- Function Prototypes ---
void usbPutString(char *s);
void usbPutChar(char c);
void handle_usb(void);
static void dbg(const char *msg);

/* Timer ISR */
CY_ISR(Timer_TS_ISR_Handler)
{
    timer_flag = 1;   
}

// --- Helpers ---
#define S_ACTIVE(pin_read) ((pin_read) == 0u)   // active-low -> 1 when line

// --- USB Debug Output Helper ---
static void dbg(const char *msg)
{
    /*
    #ifdef USE_USB
        while (USBUART_CDCIsReady() == 0) {}
        USBUART_PutString(msg);
    #else
        (void)msg;
    #endif
    */
    (void)msg;
}

/* ================= Main ================= */
int main(void)
{
    CyGlobalIntEnable;

    PWM_1_Start();
    PWM_2_Start();
    
    QuadDec_M1_Start();
    QuadDec_M2_Start();

    isr_1_StartEx(Timer_TS_ISR_Handler);
    Timer_TS_Start();

    /*
    #ifdef USE_USB
        USBUART_Start(0, USBUART_5V_OPERATION);
        dbg("\r\n=== USB Debug Interface Initialized ===\r\n");
    #endif
    */

    RobotInstr instructions[MAX_INSTRUCTIONS];
    int food_dists[5];
    int food_axes[5];
    int num_instructions = generate_instructions_from_map(instructions, MAX_INSTRUCTIONS, food_dists, food_axes);

    CyDelay(1000);
    // --- Execute plan ---
    for (int i = 0; i < num_instructions; ++i)
    {
        switch (instructions[i].type)
        {
            case iSTRAIGHT:
                if (instructions[i+1].type == iSTOP)
                {
                    if (food_index < 5)
                    {
                        run_for_food(food_dists[food_index], food_axes[food_index]);
                        food_index++;
                    }
                } 
                else
                {
                    move_forward_until_intersection();
                }
                break;

            case iLEFT:
                turn_left_enc();
                break;

            case iRIGHT:
                turn_right_enc();
                break;

            case iTURN_AROUND:
                u_turn_enc();
                break;

            case iSTOP:
                //stop();
                //CyDelay(1000);
                break;

            default:
                stop();
                CyDelay(2000);
                break;
        }
    }

    // --- Idle ---
    for(;;)
    { 
        PWM_1_WriteCompare(255); 
        PWM_2_WriteCompare(127); 
    }
}

// ==========================================================
// USB Communication Functions (Disabled)
// ==========================================================
void usbPutString(char *s)
{
    (void)s;
    /*
    #ifdef USE_USB
        while (USBUART_CDCIsReady() == 0) {}
        USBUART_PutData((uint8*)s, (uint16)strlen(s));
    #else
        (void)s;
    #endif
    */
}

void usbPutChar(char c)
{
    (void)c;
    /*
    #ifdef USE_USB
        while (USBUART_CDCIsReady() == 0) {}
        USBUART_PutChar((uint8)c);
    #else
        (void)c;
    #endif
    */
}

// ==========================================================
// USB Command Handler (Disabled)
// ==========================================================
void handle_usb(void)
{
    (void)0;
    /*
    #ifdef USE_USB
        static uint8 usbStarted = FALSE;
        static uint16 usbBufCount = 0;

        if (!usbStarted)
        {
            if (USBUART_GetConfiguration())
            {
                USBUART_CDC_Init();
                usbStarted = TRUE;
                dbg("[USB] Configuration initialized\r\n");
            }
            return;
        }

        if (USBUART_DataIsReady() == 0)
            return;

        uint8 c = USBUART_GetChar();

        if ((c == CHAR_ENTER) || (c == '\n'))
        {
            entry[usbBufCount] = '\0';
            strcpy(line, entry);
            usbBufCount = 0;
            flag_KB_string = 1;
        }
        else
        {
            if (((c == CHAR_BACKSP) || (c == CHAR_DEL)) && (usbBufCount > 0))
            {
                usbBufCount--;
            }
            else if (usbBufCount < (BUF_SIZE - 2))
            {
                entry[usbBufCount++] = (char)c;
            }
        }
    #endif
    */
}
