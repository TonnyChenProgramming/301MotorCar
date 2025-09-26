/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#include "defines.h"

#ifndef MAIN_H
#define MAIN_H
    
typedef struct edge_pack {
    uint8_t front_left_edge;
    uint8_t front_right_edge;
    uint8_t mid_left_edge;
    uint8_t mid_right_edge;
} edge_pack_t;
extern edge_pack_t edges;
/* [] END OF FILE */
#endif