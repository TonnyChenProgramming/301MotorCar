/*******************************************************************************
* File Name: front_left.h
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
#if !defined(CY_ISR_front_left_H)
#define CY_ISR_front_left_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void front_left_Start(void);
void front_left_StartEx(cyisraddress address);
void front_left_Stop(void);

CY_ISR_PROTO(front_left_Interrupt);

void front_left_SetVector(cyisraddress address);
cyisraddress front_left_GetVector(void);

void front_left_SetPriority(uint8 priority);
uint8 front_left_GetPriority(void);

void front_left_Enable(void);
uint8 front_left_GetState(void);
void front_left_Disable(void);

void front_left_SetPending(void);
void front_left_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the front_left ISR. */
#define front_left_INTC_VECTOR            ((reg32 *) front_left__INTC_VECT)

/* Address of the front_left ISR priority. */
#define front_left_INTC_PRIOR             ((reg8 *) front_left__INTC_PRIOR_REG)

/* Priority of the front_left interrupt. */
#define front_left_INTC_PRIOR_NUMBER      front_left__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable front_left interrupt. */
#define front_left_INTC_SET_EN            ((reg32 *) front_left__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the front_left interrupt. */
#define front_left_INTC_CLR_EN            ((reg32 *) front_left__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the front_left interrupt state to pending. */
#define front_left_INTC_SET_PD            ((reg32 *) front_left__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the front_left interrupt. */
#define front_left_INTC_CLR_PD            ((reg32 *) front_left__INTC_CLR_PD_REG)


#endif /* CY_ISR_front_left_H */


/* [] END OF FILE */
