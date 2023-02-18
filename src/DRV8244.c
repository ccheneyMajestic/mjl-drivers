/***************************************************************************
*                             Majestic Labs © 2022
*
* File: DRV8244.c
* Workspace: mjl_drivers
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: Driver functions for the DRV8244 Full bridge driver 
*   
*
* 2022.04.02  - Document Created
********************************************************************************/
#include "DRV8244.h"
#include "project.h"

/*******************************************************************************
* Function Name: drv8244_init()
****************************************************************************//**
* \brief
*   Sets the configuration for the 
*
* \param state [in/out]
*  
*
* \return
*   Error code of the operation
*******************************************************************************/
uint32_t drv8244_init(drv8244_state_s *const state, drv8244_config_s *const cfg){
    (void) cfg; 
    uint32_t error = 0;
    // TODO: Input validation
    // TODO: object validation

    /* Initilaize state object */
    state->_isObjectInitialized = false;
    if(false == error){
        /* Set state defaults */
        state->_isObjectLocked = true;
        state->isRunning = false;


        // TODO: Copy input paratmeters
        /* Initizize the state */
        state->state_previous = DRV8244_STATE_OFF;
        state->state_current = DRV8244_STATE_OFF;
        state->state_next = DRV8244_STATE_OFF;
        state->isEnabled = false;

        // TODO: Initialize flags and status variables
        // TODO: Initialize actuator variables
        // TODO: Initialize sensor variables

        /* pre-initize the state object */
        state->_isObjectInitialized = true;
        state->_isObjectLocked = false;

        error |= drv8244_stop(state);

        // TODO: perform nested state initization

        state->status = 0;
        state->output = 0;
        state->time_locked = 0;
        state->time_running = 0;
        state->time_lastUpdate = 0;




        // TODO: check for errors
        if(true == error){
            state->_isObjectInitialized = false;
            state->output= error;
        }
    }

    return error;    
}


/*******************************************************************************
* Function Name: drv8244_start()
****************************************************************************//**
* \brief
*   
*
* \param state [in/out]
*  
*
* \return
*   Error code of the operation
*******************************************************************************/
uint32_t drv8244_start(drv8244_state_s *const state){
    uint32_t error = 0;
    if(false == state->_isObjectInitialized){
        error |= DRV84244_ERROR_INIT;
    }
    if(true == state->_isObjectLocked) {
        error |= DRV84244_ERROR_LOCKED;
    }
    if(false == error) {
        /* Disable interrupts */
       uint32_t previous_interruptState = Cy_SysLib_EnterCriticalSection();
        /* Lock the state object */
        state->_isObjectLocked = true;

        /* Sleep the Driver */
        Cy_GPIO_Write(pin_DRV_nSLEEP_PORT, pin_DRV_nSLEEP_0_NUM, DRV8244_VAL_ASLEEP);
        /* HW: Mandatory sleep delay period */
        CyDelayUs(DRV8244_PERIOD_SLEEP);
        /* Wake the Driver */
        Cy_GPIO_Write(pin_DRV_nSLEEP_PORT, pin_DRV_nSLEEP_0_NUM, DRV8244_VAL_AWAKE);
        CyDelayUs(DRV8244_PERIOD_WAKE);
        /* Clear the awake fault */
        Cy_GPIO_Write(pin_DRV_nSLEEP_PORT, pin_DRV_nSLEEP_0_NUM, DRV8244_VAL_ASLEEP);
        CyDelayUs(DRV8244_PERIOD_RESET); /* HW: Mandatory reset fault delay period */
        Cy_GPIO_Write(pin_DRV_nSLEEP_PORT, pin_DRV_nSLEEP_0_NUM, DRV8244_VAL_AWAKE);
        
        /* Read nFault */
        CyDelayUs(DRV8244_PERIOD_WAKE);
        bool nFault = Cy_GPIO_Read(pin_DRV_nFAULT_PORT, pin_DRV_nFAULT_0_NUM);
        if(DRV8244_VAL_FAULT == nFault) {
            error |= DRV84244_ERROR_FAULT;
        }
        /* Indicate success */
        if(false == error){
            state->isRunning = true;
            /* Release lock and stop the device */
            state->_isObjectLocked = false;
        }
        else {
            error |= drv8244_stop(state);
        }
        /* Release lock */
        state->_isObjectLocked = false;
        /* Re-enable interrupts */
       Cy_SysLib_ExitCriticalSection(previous_interruptState);
    }
    return error;    
}

/*******************************************************************************
* Function Name: drv8244_stop()
****************************************************************************//**
* \brief
*   Puts the device to sleep
*
* \param state [in/out]
*  
*
* \return
*   Error code of the operation
*******************************************************************************/
uint32_t drv8244_stop(drv8244_state_s *const state){
    uint32_t error = 0;
        /* Error check  */
    if(false == state->_isObjectInitialized){
        error |= DRV84244_ERROR_INIT;
    }
    if(true == state->_isObjectLocked) {
        error |= DRV84244_ERROR_LOCKED;
    }
    if(false == error) {
        /* Disable interrupts */
        uint32_t previous_interruptState = Cy_SysLib_EnterCriticalSection();
        /* Lock the state object */
        state->_isObjectLocked = true;
        /* disable the object */
        state->_isObjectLocked = false;
        error |= drv8244_disable(state);

        /* Sleep the Driver */
        Cy_GPIO_Write(pin_DRV_nSLEEP_PORT, pin_DRV_nSLEEP_0_NUM, DRV8244_VAL_ASLEEP);
        
        /* Read back */
        bool readVal = Cy_GPIO_ReadOut(pin_DRV_nSLEEP_PORT, pin_DRV_nSLEEP_0_NUM);
        if(DRV8244_VAL_ASLEEP != readVal) {
            error |= DRV84244_ERROR_SYNC;
        }
        /* Indicate success */
        if(false == error){
            state->isRunning = false;
        }
        /* Release lock */
        state->_isObjectLocked = false;
        /* Re-enable interrupts */
        Cy_SysLib_ExitCriticalSection(previous_interruptState);
    }
    return error;    
}

/*******************************************************************************
* Function Name: drv8244_enable()
****************************************************************************//**
* \brief
*   Puts the device to sleep
*
* \param state [in/out]
*  
*
* \return
*   Error code of the operation
*******************************************************************************/
  uint32_t drv8244_enable(drv8244_state_s *const state){
    uint32_t error = 0;
    /* Error check  */
    if(false == state->_isObjectInitialized){
        error |= DRV84244_ERROR_INIT;
    }
    if(true == state->_isObjectLocked) {
        error |= DRV84244_ERROR_LOCKED;
    }
    /* No error, proceed with enabling */
    if(false == error) {
        /* Disable interrupts */
        uint32_t previous_interruptState = Cy_SysLib_EnterCriticalSection();
        /* Lock the state object */
        state->_isObjectLocked = true;

        /* Deassert the DRVOFF pin */
        uint8_t writeVal = DRV8244_VAL_ENABLED;
        Cy_GPIO_Write(pin_DRV_DRVOFF_PORT, pin_DRV_DRVOFF_0_NUM, writeVal);
        /* Read back */
        bool readVal = Cy_GPIO_ReadOut(pin_DRV_DRVOFF_PORT, pin_DRV_DRVOFF_0_NUM);
        if(writeVal != readVal) {
            error |= DRV84244_ERROR_SYNC;
        }
        /* Indicate success */
        if(false == error){
            state->isEnabled = true;
        }
        /* Release lock */
        state->_isObjectLocked = false;
        /* Re-enable interrupts */
        Cy_SysLib_ExitCriticalSection(previous_interruptState);
    }
    return error;    
}

/*******************************************************************************
* Function Name: drv8244_disable()
****************************************************************************//**
* \brief
*   Puts the device to sleep
*
* \param state [in/out]
*  
*
* \return
*   Error code of the operation
*******************************************************************************/
  uint32_t drv8244_disable(drv8244_state_s *const state){
    uint32_t error = 0;
    if(false == state->_isObjectInitialized){
        error |= DRV84244_ERROR_INIT;
    }
    if(true == state->_isObjectLocked) {
        error |= DRV84244_ERROR_LOCKED;
    }
    if(false == error) {
        /* Disable interrupts */
        uint32_t previous_interruptState = Cy_SysLib_EnterCriticalSection();
        /* Lock the state object */
        state->_isObjectLocked = true;

        /* Assert the DRVOFF pin */
        uint8_t writeVal = DRV8244_VAL_DISABLED;
        Cy_GPIO_Write(pin_DRV_DRVOFF_PORT, pin_DRV_DRVOFF_0_NUM, writeVal);
        /* Read back */
        bool readVal = Cy_GPIO_ReadOut(pin_DRV_DRVOFF_PORT, pin_DRV_DRVOFF_0_NUM);
        if(writeVal != readVal) {
            error |= DRV84244_ERROR_SYNC;
        }
        /* Indicate success */
        if(false == error){
            state->isEnabled = false;
        }
        /* Release lock */
        state->_isObjectLocked = false;
        /* Re-enable interrupts */
        Cy_SysLib_ExitCriticalSection(previous_interruptState);
    }
    return error;    
}

/* [] END OF FILE */