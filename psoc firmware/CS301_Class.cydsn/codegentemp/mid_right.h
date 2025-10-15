/*******************************************************************************
* File Name: mid_right.h
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
#if !defined(CY_ISR_mid_right_H)
#define CY_ISR_mid_right_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void mid_right_Start(void);
void mid_right_StartEx(cyisraddress address);
void mid_right_Stop(void);

CY_ISR_PROTO(mid_right_Interrupt);

void mid_right_SetVector(cyisraddress address);
cyisraddress mid_right_GetVector(void);

void mid_right_SetPriority(uint8 priority);
uint8 mid_right_GetPriority(void);

void mid_right_Enable(void);
uint8 mid_right_GetState(void);
void mid_right_Disable(void);

void mid_right_SetPending(void);
void mid_right_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the mid_right ISR. */
#define mid_right_INTC_VECTOR            ((reg32 *) mid_right__INTC_VECT)

/* Address of the mid_right ISR priority. */
#define mid_right_INTC_PRIOR             ((reg8 *) mid_right__INTC_PRIOR_REG)

/* Priority of the mid_right interrupt. */
#define mid_right_INTC_PRIOR_NUMBER      mid_right__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable mid_right interrupt. */
#define mid_right_INTC_SET_EN            ((reg32 *) mid_right__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the mid_right interrupt. */
#define mid_right_INTC_CLR_EN            ((reg32 *) mid_right__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the mid_right interrupt state to pending. */
#define mid_right_INTC_SET_PD            ((reg32 *) mid_right__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the mid_right interrupt. */
#define mid_right_INTC_CLR_PD            ((reg32 *) mid_right__INTC_CLR_PD_REG)


#endif /* CY_ISR_mid_right_H */


/* [] END OF FILE */
