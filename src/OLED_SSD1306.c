/***************************************************************************
*                             Majestic Labs © 2023
*
* File: OLED_SSD1306.h
* Workspace: MJL Driver Library 
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: Driver library for the SSD1306 OLED Display 128x64
*   
*
* 2023.02.18  - Document Created
********************************************************************************/
#include "OLED_SSD1306.h"

/*******************************************************************************
* Function Name: ssd1306_init()
****************************************************************************//**
* \brief
*  Initialize the object hardware 
*
* \param state [in/out]
*  Pointer to the state struct  
*
* \param cfg [in]
*  Pointer to the configuration struct  
*
* \return
*  Error of the command
*******************************************************************************/
uint32_t ssd1306_init(ssd1306_state_s *const state, ssd1306_cfg_s *const cfg){
    uint32_t error = 0;
    /* Input validation */
    bool isInvalidPointer = false;
    isInvalidPointer |= (NULL == cfg->fn_getTime_ms) ? true : false;
    /* Objects */
    if(true == isInvalidPointer) {
        error |= ERROR_INPUT;
    }

    if(0u == error) {
        /* copy input params */
        state->fn_getTime_ms = cfg->fn_getTime_ms;
        /* Set defaults */
        state->stateCount = 0;
        state->state_current = SSD1306_STATE_OFF;
        state->state_next = SSD1306_STATE_OFF;
        state->state_previous = SSD1306_STATE_OFF;
        state->flag_isStateTransitionRequested = false;
        state->flag_didStateTransitionOccur = false;
        state->stateTransitionCause = SSD1306_TRANSITION_NONE;
        /* Flags and status */
        state->flag_isStateTransitionRequested = false;
        state->flag_didStateTransitionOccur = false;
        /* Actuator */
        /* Sensors */

        /* Pre-initialize */
        state->_isInitialized = true;
        /* Take initial actions */

        if(0u != error) {
            state->_isInitialized = false;
            state->output = error;
        } else {
            /* Indicate success */
            state->output = 0;
        }
    }
    else {
        state->_isInitialized = false;
    }
    return error;
}

/*******************************************************************************
* Function Name: ssd1306_start()
****************************************************************************//**
* \brief
*  Initialize the object hardware 
*
* \param state [in/out]
*  Pointer to the state struct  
*
* \param cfg [in]
*  Pointer to the configuration struct  
*
* \return
*  Error of the command
*******************************************************************************/
uint32_t ssd1306_start(ssd1306_state_s *const state) {
  uint32_t error = 0;
  if(false == state->_isInitialized) {
      error |= ERROR_INIT;
  }
  if(true == state->_isLocked){
      error |= ERROR_LOCKED;
  }

  if(0 == error){
      /* Lock the object */
      state->_isLocked = true;
      // configure the state object as required
      /* Unlock the object */
      state->_isLocked = false;
      /* Perform state actions */

      /* Error check */

  }

  return error;
}


/* [] END OF FILE */