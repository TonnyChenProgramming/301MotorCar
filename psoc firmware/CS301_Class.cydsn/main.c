// ========================================
// Main File – With Debugging via USB (Putty)
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
#define USE_USB 1

// --- Globals ---
static volatile int16 left_wheel_val;
static volatile int16 right_wheel_val;
uint8_t timer_flag = 0;
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
#ifdef USE_USB
    while (USBUART_CDCIsReady() == 0) {}
    USBUART_PutString(msg);
#else
    (void)msg;
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

    isr_1_StartEx(Timer_TS_ISR_Handler);
    Timer_TS_Start();

#ifdef USE_USB
    USBUART_Start(0, USBUART_5V_OPERATION);
    dbg("\r\n=== USB Debug Interface Initialized ===\r\n");
#endif

    dbg("=== Generating instructions from map ===\r\n");

    RobotInstr instructions[MAX_INSTRUCTIONS];
    int food_dists[5];
    int num_instructions = generate_instructions_from_map(instructions, MAX_INSTRUCTIONS, food_dists);

    // --- Print plan for debugging ---
    dbg("\r\n=== Instruction Plan ===\r\n");
    for (int i = 0; i < num_instructions; ++i) {
        switch (instructions[i].type) {
            case iSTRAIGHT:     dbg("Plan: STRAIGHT\r\n"); break;
            case iLEFT:         dbg("Plan: LEFT\r\n"); break;
            case iRIGHT:        dbg("Plan: RIGHT\r\n"); break;
            case iTURN_AROUND:  dbg("Plan: TURN-AROUND\r\n"); break;
            case iSTOP:         dbg("Plan: STOP\r\n"); break;
            default:            dbg("Plan: UNKNOWN\r\n"); break;
        }
    }

    dbg("\r\n=== Food Distances ===\r\n");
    for (int i = 0; i < 5; ++i) {
        char msg[32];
        sprintf(msg, "Food %d: %d\r\n", i, food_dists[i]);
        dbg(msg);
    }

    dbg("\r\n=== Beginning Execution ===\r\n");

    // --- Execute plan ---
    for (int i = 0; i < num_instructions; ++i)
    {
        char msg[64];
        sprintf(msg, "\r\n[STATE] Executing instruction %d: ", i);
        dbg(msg);

        switch (instructions[i].type)
        {
            case iSTRAIGHT:
                dbg("STRAIGHT\r\n");
                move_forward_until_intersection();
                dbg("[DONE] Straight movement complete\r\n");
                break;

            case iLEFT:
                dbg("LEFT TURN\r\n");
                turn_left_enc();
                dbg("[DONE] Left turn complete\r\n");
                break;

            case iRIGHT:
                dbg("RIGHT TURN\r\n");
                turn_right_enc();
                dbg("[DONE] Right turn complete\r\n");
                break;

            case iTURN_AROUND:
                dbg("TURN-AROUND\r\n");
                turn_right_enc();
                turn_right_enc();
                dbg("[DONE] U-turn complete\r\n");
                break;

            case iSTOP:
                dbg("STOP\r\n");
                stop();
                CyDelay(1000);
                dbg("[DONE] Stop complete\r\n");
                break;

            default:
                dbg("UNKNOWN INSTRUCTION\r\n");
                stop();
                break;
        }

        // Optional short delay between instructions for clarity
        CyDelay(250);
    }

    dbg("\r\n=== Path Complete – Entering Idle Mode ===\r\n");

    // --- Idle (hold motors steady) ---
    for(;;)
    { 
        PWM_1_WriteCompare(255); 
        PWM_2_WriteCompare(127); 
    }
}

// ==========================================================
// USB Communication Functions
// ==========================================================
void usbPutString(char *s)
{
#ifdef USE_USB
    while (USBUART_CDCIsReady() == 0) {}
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

// ==========================================================
// USB Command Handler (for keyboard/Putty input, optional)
// ==========================================================
void handle_usb(void)
{
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
}
