/***************************************************************************
*                                Majestic Labs © 2023
* File: butterworth.c
* Workspace: MJL Driver Library 
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: Header for an IIR Butterworth Filter  
*
* 2023.06.04  - Document Created
********************************************************************************/
#include "butterworth.h" 
#include "math.h"
/* Default config struct */
const BUTTERWORTH_CFG_S butterworth_cfg_default = {
  .cornerFreq = 0,
  .qFactor = 0,
  .sampleRate = 0,
};

/*******************************************************************************
* Function Name: butterworth_init()
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
uint32_t butterworth_init(BUTTERWORTH_S *const state, BUTTERWORTH_CFG_S *const cfg){
  uint32_t error = 0;
  /* Verify required functions */
  error |= (0 == cfg->cornerFreq) ? ERROR_VAL : ERROR_NONE;
  error |= (0 == cfg->qFactor) ? ERROR_VAL : ERROR_NONE;
  error |= (0 == cfg->sampleRate) ? ERROR_VAL : ERROR_NONE;
  /* external start is not required */
  /* external stop is not required */
  /* Valid Inputs */
  if(!error) {
    /* Copy params */
    state->cornerFreq = cfg->cornerFreq;
    state->qFactor = cfg->qFactor;
    state->sampleRate =  cfg->sampleRate;
    /* Calculate the filter coefficients */
    int32_t omega = 2 * M_PI * state->cornerFreq/state->sampleRate;
    int32_t alpha = sin(omega) / (2 * state->qFactor);
    int32_t cosw = cos(omega);
    int32_t beta = (1-cosw) / 2;
    /* Set the numerator coefficients */
    state->numerator[0] = beta;
    state->numerator[1] = 2 * beta;
    state->numerator[2] = beta;
    state->numerator[3] = 2 * beta;
    state->numerator[4] = beta;
    /* Set the denomicator coeffecients */
    state->denominator[0] = 1 + alpha;
    state->denominator[1] = -2*cosw;
    state->denominator[2] = 1 - alpha;
    state->denominator[3] = -state->denominator[1];
    state->denominator[4] = -state->denominator[0];
    /* Set the input and outputs */
    state->inputLine[0] = 0;
    state->inputLine[1] = 0;
    state->inputLine[2] = 0;
    state->inputLine[3] = 0;
    state->outputLine[0] = 0;
    state->outputLine[1] = 0;
    state->outputLine[2] = 0;
    state->outputLine[3] = 0;
    state->result = 0;

    /* Mark as initialized */
    state->_init = true;
  }
  if(error){state->_init=false;}
  return error;
}

/*******************************************************************************
* Function Name: butterworth_applyInput()
********************************************************************************
* \brief
*   Applies the filter to a new sample input 
*
* \param state [in/out]
* Pointer to the state struct
*
* \param sample [in]
* New sampple 
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t butterworth_applyInput(BUTTERWORTH_S *const state, int16_t sample){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}

  if(!error){
    /* Shift the input */
    for(uint8_t i=3; i>0; i--){
      state->inputLine[i] = state->inputLine[i-1];
    }
    state->inputLine[0] = sample;
    /* Shift the output */
    for(uint8_t i=3; i>0; i--){
      state->outputLine[i] = state->outputLine[i-1];
    }
    /* Compute the filtered output */
    int32_t y = 0;
    for(uint8_t i=0; i<5; i++){
      y+= (state->numerator[i] * state->inputLine[i]);
      y-= (state->denominator[i] * state->outputLine[i]);
    }
    /* Scale down the result and round to nearest integer */
    y = (y + (1<<14)) >> 15;
    /* Clip to a int16 */
    if(y > INT16_MAX) {y=INT16_MAX;}
    else if (y < INT16_MIN) {y=INT16_MIN;}
    /* Update the output delay line */
    state->outputLine[0]=y;
    state->result = (int16_t) y;
  
  }

  return error;
}

/* [] END OF FILE */