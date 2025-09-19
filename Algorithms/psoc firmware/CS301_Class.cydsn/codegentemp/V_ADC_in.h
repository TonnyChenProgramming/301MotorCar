/*******************************************************************************
* File Name: V_ADC_in.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_V_ADC_in_H) /* Pins V_ADC_in_H */
#define CY_PINS_V_ADC_in_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "V_ADC_in_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 V_ADC_in__PORT == 15 && ((V_ADC_in__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    V_ADC_in_Write(uint8 value);
void    V_ADC_in_SetDriveMode(uint8 mode);
uint8   V_ADC_in_ReadDataReg(void);
uint8   V_ADC_in_Read(void);
void    V_ADC_in_SetInterruptMode(uint16 position, uint16 mode);
uint8   V_ADC_in_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the V_ADC_in_SetDriveMode() function.
     *  @{
     */
        #define V_ADC_in_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define V_ADC_in_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define V_ADC_in_DM_RES_UP          PIN_DM_RES_UP
        #define V_ADC_in_DM_RES_DWN         PIN_DM_RES_DWN
        #define V_ADC_in_DM_OD_LO           PIN_DM_OD_LO
        #define V_ADC_in_DM_OD_HI           PIN_DM_OD_HI
        #define V_ADC_in_DM_STRONG          PIN_DM_STRONG
        #define V_ADC_in_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define V_ADC_in_MASK               V_ADC_in__MASK
#define V_ADC_in_SHIFT              V_ADC_in__SHIFT
#define V_ADC_in_WIDTH              1u

/* Interrupt constants */
#if defined(V_ADC_in__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in V_ADC_in_SetInterruptMode() function.
     *  @{
     */
        #define V_ADC_in_INTR_NONE      (uint16)(0x0000u)
        #define V_ADC_in_INTR_RISING    (uint16)(0x0001u)
        #define V_ADC_in_INTR_FALLING   (uint16)(0x0002u)
        #define V_ADC_in_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define V_ADC_in_INTR_MASK      (0x01u) 
#endif /* (V_ADC_in__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define V_ADC_in_PS                     (* (reg8 *) V_ADC_in__PS)
/* Data Register */
#define V_ADC_in_DR                     (* (reg8 *) V_ADC_in__DR)
/* Port Number */
#define V_ADC_in_PRT_NUM                (* (reg8 *) V_ADC_in__PRT) 
/* Connect to Analog Globals */                                                  
#define V_ADC_in_AG                     (* (reg8 *) V_ADC_in__AG)                       
/* Analog MUX bux enable */
#define V_ADC_in_AMUX                   (* (reg8 *) V_ADC_in__AMUX) 
/* Bidirectional Enable */                                                        
#define V_ADC_in_BIE                    (* (reg8 *) V_ADC_in__BIE)
/* Bit-mask for Aliased Register Access */
#define V_ADC_in_BIT_MASK               (* (reg8 *) V_ADC_in__BIT_MASK)
/* Bypass Enable */
#define V_ADC_in_BYP                    (* (reg8 *) V_ADC_in__BYP)
/* Port wide control signals */                                                   
#define V_ADC_in_CTL                    (* (reg8 *) V_ADC_in__CTL)
/* Drive Modes */
#define V_ADC_in_DM0                    (* (reg8 *) V_ADC_in__DM0) 
#define V_ADC_in_DM1                    (* (reg8 *) V_ADC_in__DM1)
#define V_ADC_in_DM2                    (* (reg8 *) V_ADC_in__DM2) 
/* Input Buffer Disable Override */
#define V_ADC_in_INP_DIS                (* (reg8 *) V_ADC_in__INP_DIS)
/* LCD Common or Segment Drive */
#define V_ADC_in_LCD_COM_SEG            (* (reg8 *) V_ADC_in__LCD_COM_SEG)
/* Enable Segment LCD */
#define V_ADC_in_LCD_EN                 (* (reg8 *) V_ADC_in__LCD_EN)
/* Slew Rate Control */
#define V_ADC_in_SLW                    (* (reg8 *) V_ADC_in__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define V_ADC_in_PRTDSI__CAPS_SEL       (* (reg8 *) V_ADC_in__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define V_ADC_in_PRTDSI__DBL_SYNC_IN    (* (reg8 *) V_ADC_in__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define V_ADC_in_PRTDSI__OE_SEL0        (* (reg8 *) V_ADC_in__PRTDSI__OE_SEL0) 
#define V_ADC_in_PRTDSI__OE_SEL1        (* (reg8 *) V_ADC_in__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define V_ADC_in_PRTDSI__OUT_SEL0       (* (reg8 *) V_ADC_in__PRTDSI__OUT_SEL0) 
#define V_ADC_in_PRTDSI__OUT_SEL1       (* (reg8 *) V_ADC_in__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define V_ADC_in_PRTDSI__SYNC_OUT       (* (reg8 *) V_ADC_in__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(V_ADC_in__SIO_CFG)
    #define V_ADC_in_SIO_HYST_EN        (* (reg8 *) V_ADC_in__SIO_HYST_EN)
    #define V_ADC_in_SIO_REG_HIFREQ     (* (reg8 *) V_ADC_in__SIO_REG_HIFREQ)
    #define V_ADC_in_SIO_CFG            (* (reg8 *) V_ADC_in__SIO_CFG)
    #define V_ADC_in_SIO_DIFF           (* (reg8 *) V_ADC_in__SIO_DIFF)
#endif /* (V_ADC_in__SIO_CFG) */

/* Interrupt Registers */
#if defined(V_ADC_in__INTSTAT)
    #define V_ADC_in_INTSTAT            (* (reg8 *) V_ADC_in__INTSTAT)
    #define V_ADC_in_SNAP               (* (reg8 *) V_ADC_in__SNAP)
    
	#define V_ADC_in_0_INTTYPE_REG 		(* (reg8 *) V_ADC_in__0__INTTYPE)
#endif /* (V_ADC_in__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_V_ADC_in_H */


/* [] END OF FILE */
