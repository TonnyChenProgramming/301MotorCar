// Main File 

// Including all header files
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
#include "map_to_instructions.h" 



#define TIMER_BASE_HZ 100000UL

 

static volatile int16 left_wheel_val; // positive
static volatile int16 right_wheel_val;//negative

static void motor_left(uint16 val)  { PWM_1_WriteCompare(val); }
static void motor_right(uint16 val) { PWM_2_WriteCompare(val); }

uint8_t timer_flag = 0;
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

    #ifdef USE_USB
    USBUART_Start(0, USBUART_5V_OPERATION);
    #endif

    RobotInstr instructions[MAX_INSTRUCTIONS];
    int food_dists[5];
    int num_instructions = generate_instructions_from_map(instructions, MAX_INSTRUCTIONS, food_dists);


    
// --- Print plan ---
for (int i = 0; i < num_instructions; ++i) {
    switch (instructions[i].type) {
        case iSTRAIGHT:     usbPutString("Plan: straight until intersection\r\n"); break;
        case iLEFT:         usbPutString("Plan: left\r\n"); break;
        case iRIGHT:        usbPutString("Plan: right\r\n"); break;
        case iTURN_AROUND:  usbPutString("Plan: turn-around\r\n"); break;
        case iSTOP:         usbPutString("Plan: stop\r\n"); break;
        default:            usbPutString("Plan: unknown\r\n"); break;
    }
}

usbPutString("=== Food distances ===\r\n");
for (int i = 0; i < 5; ++i) {
    char msg[32];
    sprintf(msg, "Food %d: %d\r\n", i, food_dists[i]);
    usbPutString(msg);
}


    // Wait 5 seconds before starting movement
  //  CyDelay(5000);

    // Execute plan
    // --- Execute plan ---
for (int i = 0; i < num_instructions; ++i) {
    switch (instructions[i].type) {
        case iSTRAIGHT:
            move_forward_until_intersection();
            break;
        case iLEFT:
            turn_left_until_line();
            break;
        case iRIGHT:
            turn_right_until_line();
            break;
        case iTURN_AROUND:
            turn_right_until_line();
            turn_right_until_line();
            break;
        case iSTOP:
            stop();
            CyDelay(1000);
            break;
        default:
            stop();
            break;
    }
}

    // Idle after execution
    for(;;) { PWM_1_WriteCompare(255); PWM_2_WriteCompare(127); }
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

