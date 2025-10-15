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

volatile uint16 tick_count = 0;      // increases in ISR
volatile uint8 delay_active = 0;     // 1 = we are pausing
volatile uint16 delay_target = 0;    // how long to pause (in ticks)


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

static void motor_left(uint16 val)  { PWM_1_WriteCompare(val); }
static void motor_right(uint16 val) { PWM_2_WriteCompare(val); }

uint8_t timer_flag = 0;
edge_pack_t edges = {0,0,0,0, 0, 0};
MovementFiniteState movement_finite_state = STOP_STATE;
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
    
    QuadDec_M1_Start();
    QuadDec_M2_Start();
    QuadDec_M1_SetCounter(0);
    QuadDec_M2_SetCounter(0);

#ifdef USE_USB
   USBUART_Start(0, USBUART_5V_OPERATION);
#endif

for(;;) {
    
    if (timer_flag) {
       timer_flag = 0;
       MovementState m = GetMovement();
      //  uint8 o1 = Output_1_Read(); // middle-right
      //      uint8 o2 = Output_2_Read(); // middle-left
      //      uint8 o3 = Output_3_Read(); // right wing
      //      uint8 o4 = Output_4_Read(); // front-right
      //      uint8 o5 = Output_5_Read(); // front-left
      //      uint8 o6 = Output_6_Read(); // left wing
      //      
                    // Remember last readings
     //   static uint8 prev_o1 = 1, prev_o2 = 1, prev_o3 = 1, prev_o4 = 1, prev_o5 = 1, prev_o6 = 1;
    //    static uint8 stable_counter = 0;
            
            
 
          // detect any change
  //      uint8 changed = (o1 != prev_o1) || (o2 != prev_o2) || (o3 != prev_o3) ||
//                        (o4 != prev_o4) || (o5 != prev_o5) || (o6 != prev_o6);

       // if (changed) {
       //     stable_counter++;
       // } else {
       //     stable_counter = 0;
       // }      
         //   if (stable_counter == 3) {   
         //   char buf[120];
         //   sprintf(buf, "mid right:%d  midleft:%d  right wing:%d  front right:%d  front left:%d  left wing:%d\r\n",
        //            o1, o2, o3, o4, o5, o6);
        //    usbPutString(buf);
            
        //     prev_o1 = o1;
        //    prev_o2 = o2;
        //    prev_o3 = o3;
        //    prev_o4 = o4;
         //   prev_o5 = o5;
        //    prev_o6 = o6;
        //    
      //      stable_counter = 0;
    //        }
  //  }
            //  if (timer_flag) {
    //    timer_flag = 0;
      //  move_handling();   // runs every 5 ms
  //  }
        
                     // Debug print current state
      //  char buf[64];
       // switch(m)
        //{
         //   case STOP:        sprintf(buf, "STATE: STOP\r\n"); break;
          //  case STRAIGHT:    sprintf(buf, "STATE: STRAIGHT\r\n"); break;
           // case LEFT_TURN:   sprintf(buf, "STATE: LEFT TURN\r\n"); break;
          //  case RIGHT_TURN:  sprintf(buf, "STATE: RIGHT TURN\r\n"); break;
           // case WAIT:        sprintf(buf, "STATE: WAIT\r\n"); break;
          //  default:          sprintf(buf, "STATE: UNKNOWN\r\n"); break;
        //}
        //usbPutString(buf);

    //    CyDelay(500); // small delay so it doesn't spam the terminal
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

