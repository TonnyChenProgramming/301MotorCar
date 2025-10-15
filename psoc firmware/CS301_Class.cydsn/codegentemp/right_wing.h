/*******************************************************************************
* File Name: right_wing.h
* Version 1.71
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_right_wing_H)
#define CY_ISR_right_wing_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void right_wing_Start(void);
void right_wing_StartEx(cyisraddress address);
void right_wing_Stop(void);

CY_ISR_PROTO(right_wing_Interrupt);

void right_wing_SetVector(cyisraddress address);
cyisraddress right_wing_GetVector(void);

void right_wing_SetPriority(uint8 priority);
uint8 right_wing_GetPriority(void);

void right_wing_Enable(void);
uint8 right_wing_GetState(void);
void right_wing_Disable(void);

void right_wing_SetPending(void);
void right_wing_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the right_wing ISR. */
#define right_wing_INTC_VECTOR            ((reg32 *) right_wing__INTC_VECT)

/* Address of the right_wing ISR priority. */
#define right_wing_INTC_PRIOR             ((reg8 *) right_wing__INTC_PRIOR_REG)

/* Priority of the right_wing interrupt. */
#define right_wing_INTC_PRIOR_NUMBER      right_wing__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable right_wing interrupt. */
#define right_wing_INTC_SET_EN            ((reg32 *) right_wing__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the right_wing interrupt. */
#define right_wing_INTC_CLR_EN            ((reg32 *) right_wing__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the right_wing interrupt state to pending. */
#define right_wing_INTC_SET_PD            ((reg32 *) right_wing__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the right_wing interrupt. */
#define right_wing_INTC_CLR_PD            ((reg32 *) right_wing__INTC_CLR_PD_REG)


#endif /* CY_ISR_right_wing_H */


/* [] END OF FILE */
