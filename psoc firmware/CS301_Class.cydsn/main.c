#include "project.h"
#include <stdint.h>
#include <stdbool.h>
#include "MOVEMENT.h"
#include "SENSORS_READ.h"
#include "map.h"   // optional if you define instructions elsewhere

MovementState previous_movement = STOP;

// Example instruction sequence
// Replace this with one produced by map_to_instructions.c
uint8_t instructions[] = {STRAIGHT, LEFT_TURN, STRAIGHT, STRAIGHT,RIGHT_TURN, STOP};
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

    // brief pause to settle sensors
    CyDelay(1000);

    // Execute pre-planned path
    execute_path(instructions, num_instructions);

    // Final stop
    stop();

    for (;;)
    {
        // Idle loop — blink LED if desired
     
    }
}
