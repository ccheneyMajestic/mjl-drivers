/***************************************************************************
*                                Majestic Labs © 2023
* File: mjl_button.h
* Workspace: MJL Driver Library 
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: Button Library 
*
* 2023.04.27  - Document Created
********************************************************************************/
#include "mjl_button.h"

/* Default config struct */
const MJL_BUTTON_CFG_S button_cfg_default = {
  .fn_isBtnPressed = NULL,
};

/*******************************************************************************
* Function Name: button_init()
********************************************************************************
* \brief
*   Initializes the state struct from a configuration struct 
*
* \param state [in/out]
* Pointer to the state struct
* 
* \param cfg [in]
* Pointer to the configuration struct
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t button_init(MJL_BUTTON_S *const state, MJL_BUTTON_CFG_S *const cfg){
  uint32_t error = 0;
  /* Verify required functions */
  error |= (NULL == cfg->fn_isBtnPressed) ? ERROR_POINTER : ERROR_NONE;
  /* external start is not required */
  /* external stop is not required */
  /* Valid Inputs */
  if(!error) {
    /* Copy params */
    state->fn_isBtnPressed = cfg->fn_isBtnPressed;
    /* Set default vals */
    state->isBtnPressed = false;
    state->isBtnPressed_prev = false;
    state->startMillis = 0;
    state->elapsedMillis = 0;
    /* Mark as initialized */
    state->_init = true;
  }
  if(error){state->_init=false;}
  return error;
}

/*******************************************************************************
* Function Name: button_updateButton()
********************************************************************************
* \brief
*   Read in the latest button state 
*
* \param state [in/out]
* Pointer to the state struct
*
* \param currentMillis [in]
* Current time in [ms]
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t button_updateButton(MJL_BUTTON_S *const state, uint64_t currentMillis){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(!error){
    /* Shift state */
    state->isBtnPressed_prev = state->isBtnPressed;
    state->isBtnPressed = state->fn_isBtnPressed();
    /* Check for state transition */
    if(state->isBtnPressed != state->isBtnPressed_prev){
      if(state->isBtnPressed){
        state->startMillis = currentMillis;
        state->elapsedMillis = 0;
      }
      else{
        state->elapsedMillis = currentMillis - state->startMillis;
        state->startMillis = 0;
      }
    }
  }
  return error;
}

/* [] END OF FILE */