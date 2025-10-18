#include "project.h"
#include <stdint.h>
#include <stdbool.h>
#include "MOVEMENT.h"
#include "SENSORS_READ.h"
#include "map.h"   // optional if you define instructions elsewhere

MovementState previous_movement = STOP;
uint16_t food_distances[5] = {2,3,4,5,6};
// Example instruction sequence
// Replace this with one produced by map_to_instructions.c
uint8_t instructions[] = {STRAIGHT,  RIGHT_TURN, STRAIGHT, LEFT_TURN, STRAIGHT, LEFT_TURN, FOOD, U_TURN,STRAIGHT,RIGHT_TURN,STRAIGHT,RIGHT_TURN,STRAIGHT,LEFT_TURN,STRAIGHT,};
uint8_t num_instructions = sizeof(instructions) / sizeof(instructions[0]);

static void hardware_init(void)
{
    CyGlobalIntEnable;

    PWM_1_Start();
    PWM_2_Start();

    QuadDec_M1_Start();
    QuadDec_M2_Start();

  
}

int main(void)
{
    hardware_init();

    CyDelay(1000);
     
    
    // brief pause to settle sensors
    

    // Execute pre-planned path
    execute_path(instructions, num_instructions,food_distances);

    // Final stop
    stop();

    for (;;)
    {
        // Idle loop — blink LED if desired
     
    }
}
