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
* 2024.01.01 - Updated to MJL Library 
 ********************************************************************************/
#include "DRV8244.h"
#include "mjl_errors.h"

/* Default configuration struct */
const drv8244_cfg_s drv8244_config_default  = {
  .fn_pin_sleep_write = NULL,
  .fn_pin_drvoff_write = NULL,
  .fn_delay_us = NULL,
  .fn_criticalSection_exit = NULL,
  .fn_pin_fault_read = NULL,
  .fn_criticalSection_enter = NULL,
};

/*******************************************************************************
 * Function Name: drv8244_init()
 ********************************************************************************
 * \brief
 *   Verifies and copies the configuration structure. Does not start the device 
 *
 * \param state [in/out]
 *  
 *
 * \return
 *   Error code of the operation
 *******************************************************************************/
 uint32_t drv8244_init(drv8244_state_s *const state, drv8244_cfg_s *const cfg){
  uint32_t error = 0;
  /* Set state params */
  state->_init = false;
  state->_running = false;
  state->isEnabled = false;
  /* Verify required functions */
  error |= (NULL == cfg->fn_pin_sleep_write) ? ERROR_POINTER : ERROR_NONE;
  error |= (NULL == cfg->fn_pin_drvoff_write) ? ERROR_POINTER : ERROR_NONE;
  error |= (NULL == cfg->fn_delay_us) ? ERROR_POINTER : ERROR_NONE;
  error |= (NULL == cfg->fn_criticalSection_exit) ? ERROR_POINTER : ERROR_NONE;
  error |= (NULL == cfg->fn_pin_fault_read) ? ERROR_POINTER : ERROR_NONE;
  error |= (NULL == cfg->fn_criticalSection_enter) ? ERROR_POINTER : ERROR_NONE;
  
  if(!error) {
    /* Copy config params */
    state->fn_pin_sleep_write = cfg->fn_pin_sleep_write;
    state->fn_pin_drvoff_write = cfg->fn_pin_drvoff_write;
    state->fn_delay_us = cfg->fn_delay_us;
    state->fn_criticalSection_exit = cfg->fn_criticalSection_exit;
    state->fn_pin_fault_read = cfg->fn_pin_fault_read;
    state->fn_criticalSection_enter = cfg->fn_criticalSection_enter;
    /* Mark as initialized */
    state->_init = true;    
  }
  return error;
}

/*******************************************************************************
 * Function Name: drv8244_start()
 ********************************************************************************
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
  if(!state->_init){error|=ERROR_INIT;}
  if(state->_running){error|=ERROR_RUNNING;}

  if(!error) {
    /* Reset the device */
    error |= drv8244_stop(state);
    /* Premark as running */
    state->_running = true;
    /* Enter the critical section */
    uint32_t interruptState = state->fn_criticalSection_enter();
    /* Sleep the driver */
    state->fn_pin_sleep_write(DRV8244_VAL_ASLEEP);
    state->fn_delay_us(DRV8244_PERIOD_SLEEP); /* HW: Mandatory sleep delay period */
    /* Wake the driver */
    state->fn_pin_sleep_write(DRV8244_VAL_AWAKE);
    state->fn_delay_us(DRV8244_PERIOD_WAKE); /* HW: Mandatory boot delay period */
    /* Clear the wake fault */
    state->fn_pin_sleep_write(DRV8244_VAL_ASLEEP);
    state->fn_delay_us(DRV8244_PERIOD_RESET); /* HW: Mandatory reset fault delay period */
    state->fn_pin_sleep_write(DRV8244_VAL_AWAKE);
    state->fn_delay_us(DRV8244_PERIOD_WAKE);
    /* Ensure fault was cleared  */
    bool nFault = state->fn_pin_fault_read();
    if(DRV8244_VAL_FAULT == nFault){error|= ERROR_MODE;}
    /* Exit critical section */
    state->fn_criticalSection_exit(interruptState);
    
    if(error){state->_running = false;}
    else {state->_running = true;}
  }
  return error;  
}


/*******************************************************************************
 * Function Name: drv8244_stop()
 ********************************************************************************
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
  if(!state->_init){error|=ERROR_INIT;}
  
  if(!error){
    /* Disable output */
    error |= drv8244_disable(state);
    /* Sleep driver */
    state->fn_pin_sleep_write(DRV8244_VAL_ASLEEP);
    state->_running = false;
  }
  return error;
}

/*******************************************************************************
 * Function Name: drv8244_enable()
 ********************************************************************************
 * \brief
 *   Enables the device 
 *
 * \param state [in/out]
 *  
 *
 * \return
 *   Error code of the operation
 *******************************************************************************/
uint32_t drv8244_enable(drv8244_state_s *const state){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(!state->_running){error|=ERROR_STOPPED;}
  
  if(!error){
    state->fn_pin_drvoff_write(DRV8244_VAL_ENABLED);
    state->isEnabled = true;
  }
  return error;
}

 /*******************************************************************************
 * Function Name: drv8244_disable()
 ********************************************************************************
 * \brief
 *  Disables the device 
 *
 * \param state [in/out]
 *  
 *
 * \return
 *   Error code of the operation
 *******************************************************************************/
uint32_t drv8244_disable(drv8244_state_s *const state){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  
  if(!error){
    state->fn_pin_drvoff_write(DRV8244_VAL_DISABLED);
    state->isEnabled = false;
  }
  return error;
}
 /* [] END OF FILE */