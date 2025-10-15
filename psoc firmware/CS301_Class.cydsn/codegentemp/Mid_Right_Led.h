/*******************************************************************************
* File Name: Mid_Right_Led.h  
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

#if !defined(CY_PINS_Mid_Right_Led_H) /* Pins Mid_Right_Led_H */
#define CY_PINS_Mid_Right_Led_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Mid_Right_Led_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Mid_Right_Led__PORT == 15 && ((Mid_Right_Led__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Mid_Right_Led_Write(uint8 value);
void    Mid_Right_Led_SetDriveMode(uint8 mode);
uint8   Mid_Right_Led_ReadDataReg(void);
uint8   Mid_Right_Led_Read(void);
void    Mid_Right_Led_SetInterruptMode(uint16 position, uint16 mode);
uint8   Mid_Right_Led_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Mid_Right_Led_SetDriveMode() function.
     *  @{
     */
        #define Mid_Right_Led_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Mid_Right_Led_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Mid_Right_Led_DM_RES_UP          PIN_DM_RES_UP
        #define Mid_Right_Led_DM_RES_DWN         PIN_DM_RES_DWN
        #define Mid_Right_Led_DM_OD_LO           PIN_DM_OD_LO
        #define Mid_Right_Led_DM_OD_HI           PIN_DM_OD_HI
        #define Mid_Right_Led_DM_STRONG          PIN_DM_STRONG
        #define Mid_Right_Led_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Mid_Right_Led_MASK               Mid_Right_Led__MASK
#define Mid_Right_Led_SHIFT              Mid_Right_Led__SHIFT
#define Mid_Right_Led_WIDTH              1u

/* Interrupt constants */
#if defined(Mid_Right_Led__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Mid_Right_Led_SetInterruptMode() function.
     *  @{
     */
        #define Mid_Right_Led_INTR_NONE      (uint16)(0x0000u)
        #define Mid_Right_Led_INTR_RISING    (uint16)(0x0001u)
        #define Mid_Right_Led_INTR_FALLING   (uint16)(0x0002u)
        #define Mid_Right_Led_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Mid_Right_Led_INTR_MASK      (0x01u) 
#endif /* (Mid_Right_Led__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Mid_Right_Led_PS                     (* (reg8 *) Mid_Right_Led__PS)
/* Data Register */
#define Mid_Right_Led_DR                     (* (reg8 *) Mid_Right_Led__DR)
/* Port Number */
#define Mid_Right_Led_PRT_NUM                (* (reg8 *) Mid_Right_Led__PRT) 
/* Connect to Analog Globals */                                                  
#define Mid_Right_Led_AG                     (* (reg8 *) Mid_Right_Led__AG)                       
/* Analog MUX bux enable */
#define Mid_Right_Led_AMUX                   (* (reg8 *) Mid_Right_Led__AMUX) 
/* Bidirectional Enable */                                                        
#define Mid_Right_Led_BIE                    (* (reg8 *) Mid_Right_Led__BIE)
/* Bit-mask for Aliased Register Access */
#define Mid_Right_Led_BIT_MASK               (* (reg8 *) Mid_Right_Led__BIT_MASK)
/* Bypass Enable */
#define Mid_Right_Led_BYP                    (* (reg8 *) Mid_Right_Led__BYP)
/* Port wide control signals */                                                   
#define Mid_Right_Led_CTL                    (* (reg8 *) Mid_Right_Led__CTL)
/* Drive Modes */
#define Mid_Right_Led_DM0                    (* (reg8 *) Mid_Right_Led__DM0) 
#define Mid_Right_Led_DM1                    (* (reg8 *) Mid_Right_Led__DM1)
#define Mid_Right_Led_DM2                    (* (reg8 *) Mid_Right_Led__DM2) 
/* Input Buffer Disable Override */
#define Mid_Right_Led_INP_DIS                (* (reg8 *) Mid_Right_Led__INP_DIS)
/* LCD Common or Segment Drive */
#define Mid_Right_Led_LCD_COM_SEG            (* (reg8 *) Mid_Right_Led__LCD_COM_SEG)
/* Enable Segment LCD */
#define Mid_Right_Led_LCD_EN                 (* (reg8 *) Mid_Right_Led__LCD_EN)
/* Slew Rate Control */
#define Mid_Right_Led_SLW                    (* (reg8 *) Mid_Right_Led__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Mid_Right_Led_PRTDSI__CAPS_SEL       (* (reg8 *) Mid_Right_Led__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Mid_Right_Led_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Mid_Right_Led__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Mid_Right_Led_PRTDSI__OE_SEL0        (* (reg8 *) Mid_Right_Led__PRTDSI__OE_SEL0) 
#define Mid_Right_Led_PRTDSI__OE_SEL1        (* (reg8 *) Mid_Right_Led__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Mid_Right_Led_PRTDSI__OUT_SEL0       (* (reg8 *) Mid_Right_Led__PRTDSI__OUT_SEL0) 
#define Mid_Right_Led_PRTDSI__OUT_SEL1       (* (reg8 *) Mid_Right_Led__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Mid_Right_Led_PRTDSI__SYNC_OUT       (* (reg8 *) Mid_Right_Led__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Mid_Right_Led__SIO_CFG)
    #define Mid_Right_Led_SIO_HYST_EN        (* (reg8 *) Mid_Right_Led__SIO_HYST_EN)
    #define Mid_Right_Led_SIO_REG_HIFREQ     (* (reg8 *) Mid_Right_Led__SIO_REG_HIFREQ)
    #define Mid_Right_Led_SIO_CFG            (* (reg8 *) Mid_Right_Led__SIO_CFG)
    #define Mid_Right_Led_SIO_DIFF           (* (reg8 *) Mid_Right_Led__SIO_DIFF)
#endif /* (Mid_Right_Led__SIO_CFG) */

/* Interrupt Registers */
#if defined(Mid_Right_Led__INTSTAT)
    #define Mid_Right_Led_INTSTAT            (* (reg8 *) Mid_Right_Led__INTSTAT)
    #define Mid_Right_Led_SNAP               (* (reg8 *) Mid_Right_Led__SNAP)
    
	#define Mid_Right_Led_0_INTTYPE_REG 		(* (reg8 *) Mid_Right_Led__0__INTTYPE)
#endif /* (Mid_Right_Led__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Mid_Right_Led_H */


/* [] END OF FILE */
