/*******************************************************************************
* File Name: Turn_Timer_PM.c
* Version 2.80
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "Turn_Timer.h"

static Turn_Timer_backupStruct Turn_Timer_backup;


/*******************************************************************************
* Function Name: Turn_Timer_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Turn_Timer_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Turn_Timer_SaveConfig(void) 
{
    #if (!Turn_Timer_UsingFixedFunction)
        Turn_Timer_backup.TimerUdb = Turn_Timer_ReadCounter();
        Turn_Timer_backup.InterruptMaskValue = Turn_Timer_STATUS_MASK;
        #if (Turn_Timer_UsingHWCaptureCounter)
            Turn_Timer_backup.TimerCaptureCounter = Turn_Timer_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Turn_Timer_UDB_CONTROL_REG_REMOVED)
            Turn_Timer_backup.TimerControlRegister = Turn_Timer_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Turn_Timer_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Turn_Timer_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Turn_Timer_RestoreConfig(void) 
{   
    #if (!Turn_Timer_UsingFixedFunction)

        Turn_Timer_WriteCounter(Turn_Timer_backup.TimerUdb);
        Turn_Timer_STATUS_MASK =Turn_Timer_backup.InterruptMaskValue;
        #if (Turn_Timer_UsingHWCaptureCounter)
            Turn_Timer_SetCaptureCount(Turn_Timer_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Turn_Timer_UDB_CONTROL_REG_REMOVED)
            Turn_Timer_WriteControlRegister(Turn_Timer_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Turn_Timer_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Turn_Timer_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Turn_Timer_Sleep(void) 
{
    #if(!Turn_Timer_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Turn_Timer_CTRL_ENABLE == (Turn_Timer_CONTROL & Turn_Timer_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Turn_Timer_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Turn_Timer_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Turn_Timer_Stop();
    Turn_Timer_SaveConfig();
}


/*******************************************************************************
* Function Name: Turn_Timer_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Turn_Timer_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Turn_Timer_Wakeup(void) 
{
    Turn_Timer_RestoreConfig();
    #if(!Turn_Timer_UDB_CONTROL_REG_REMOVED)
        if(Turn_Timer_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Turn_Timer_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
