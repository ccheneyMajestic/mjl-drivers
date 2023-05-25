/***************************************************************************
*                                Majestic Labs © 2023
* File: LTC6915.h
* Workspace: MJL Driver Library 
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: Driver file for the LTC6915 Variable gain instrumentation amplifier 
*   
*
* 2023.04.25  - Document Created
********************************************************************************/
#include "LTC6915.h" 

/* Default configuration structure */
const LTC6915_cfg_s ltc6915_default = {
  .fn_pin_D0_Write = NULL, 
  .fn_pin_D1_Write = NULL,
  .fn_pin_D2_Write = NULL,
  .fn_pin_D3_Write = NULL,
  .fn_spiWriteArray = NULL,
  .mode = 0,
  .gainWord = 0,
  .slaveId = 0,
};

/*******************************************************************************
* Function Name: ltc6915_init()
********************************************************************************
* \brief
*   Initializes the LTC state struct from a configuration struct 
*
* \param state [out]
* Pointer to the state struct
* 
* \param cfg [in]
* Pointer to the configuration struct
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t ltc6915_init(LTC6915_S *const state, LTC6915_cfg_s *const cfg){
  uint32_t error = 0;
  /* Verify Communication Functions */
  if (LTC6915_MODE_SERIAL == cfg->mode){
    /* Check SPI functions */
    error |= (NULL == cfg->fn_spiWriteArray) ? ERROR_POINTER : ERROR_NONE;
    /* slaveID: No limitation on value */
  }
  /* Parallel control */
  else if(LTC6915_MODE_PARALLEL == cfg->mode){
    error |= (NULL == cfg->fn_pin_D0_Write) ? ERROR_POINTER : ERROR_NONE;
    error |= (NULL == cfg->fn_pin_D1_Write) ? ERROR_POINTER : ERROR_NONE;
    error |= (NULL == cfg->fn_pin_D2_Write) ? ERROR_POINTER : ERROR_NONE;
    error |= (NULL == cfg->fn_pin_D3_Write) ? ERROR_POINTER : ERROR_NONE;
  }
  /* Unknown mode */
  else {error |= ERROR_MODE;}
  /* Check the default gain */
  if(!ltc6915_isValidGainWord(cfg->gainWord)){error|=ERROR_VAL;}
  /* Valid Inputs */
  if(!error) {
    /* Copy into configuration structure */
    state->mode = cfg->mode;
    state->slaveId = cfg->slaveId;
    state->gainWord = cfg->gainWord;
    /* Copy the relevant communications */
    if (LTC6915_MODE_SERIAL == cfg->mode){
      state->fn_spiWriteArray = cfg->fn_spiWriteArray;
    }
    else if(LTC6915_MODE_PARALLEL == cfg->mode){
      state->fn_pin_D0_Write = cfg->fn_pin_D0_Write;
      state->fn_pin_D1_Write = cfg->fn_pin_D1_Write;
      state->fn_pin_D2_Write = cfg->fn_pin_D2_Write;
      state->fn_pin_D3_Write = cfg->fn_pin_D3_Write;
    }
    /* Confirm initilization */
    state->_init = true;
    state->_running = false;
  }
  /* Ensure no errors */
  if(error){state->_init = false;}
  return error;
}

/*******************************************************************************
* Function Name: ltc6915_start()
********************************************************************************
* \brief
*  Starts the LTC6915. Must be called after initialization. Sets the gain that
*  was set at initialization
*
* \param state [in/out]
* Pointer to the state struct
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t ltc6915_start(LTC6915_S *const state) {
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(state->_running){error|=ERROR_RUNNING;}

  if(!error){
    /* Pre-mark as running */
    state->_running = true;
    error |= ltc6915_setGainWord(state, state->gainWord);
  }
  /* Ensure no errors */
  if(error){state->_running = false;}
  return error;
}

/*******************************************************************************
* Function Name: ltc6915_stop()
********************************************************************************
* \brief
*  Sets the gain to zero and marks the struct as stopped 
*
* \param state [in/out]
* Pointer to the state struct
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t ltc6915_stop(LTC6915_S *const state) {
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(!state->_running){error|=ERROR_STOPPED;}

  if(!error){
    error |= ltc6915_setGainWord(state, LTC6915_GAIN_0);
    state->_running = false;
  }

  return error;
}

/*******************************************************************************
* Function Name: ltc6915_setGainWord()
********************************************************************************
* \brief
*   Sets the gain on the hardware with the gain word either via Serial or Parallel, 
* depending on mode
*
* \param state [in/out]
* Pointer to the state struct
* 
* \param gainWord [in]
* Pointer to the configuration struct
*
* \return
*  Error code of the operation
*******************************************************************************/
  uint32_t ltc6915_setGainWord(LTC6915_S *const state, LTC6915_GAIN_T gainWord){
    uint32_t error = 0;
    if(!state->_init){error|=ERROR_INIT;}
    if(!state->_running){error|=ERROR_STOPPED;}
    if(!ltc6915_isValidGainWord(gainWord)){error|=ERROR_VAL;}

    if(!error){
      if(LTC6915_MODE_SERIAL == state->mode){
        uint8_t data = (uint8_t) gainWord;
        error |= state->fn_spiWriteArray(state->slaveId, &data, 1);
      }
      else if (LTC6915_MODE_PARALLEL == state->mode){
        state->fn_pin_D0_Write((bool) (gainWord & LTC6915_MASK_D0));
        state->fn_pin_D1_Write((bool) (gainWord & LTC6915_MASK_D1));
        state->fn_pin_D2_Write((bool) (gainWord & LTC6915_MASK_D2));
        state->fn_pin_D3_Write((bool) (gainWord & LTC6915_MASK_D3));
      }
      else{error|=ERROR_PARAM;} 
      if(!error){state->gainWord = gainWord;}
    }
    return error;
  }


  /************************************** Utils ******************************/

  /*******************************************************************************
* Function Name: ltc6915_valueFromWord()
********************************************************************************
* \brief
*  Produces the gain value from the gain word
*
* \param gainWord [in]
*  Gain word to test 
*
* \param result [out]
*  Value of the word
*
* \return
*  Error of the operation
*******************************************************************************/
uint32_t ltc6915_valueFromWord(LTC6915_GAIN_T gainWord, uint16_t *const result){
  uint32_t error = 0;
  if(!ltc6915_isValidGainWord(gainWord)){error|=ERROR_VAL;}
  uint16_t val = 0;
  if(!error){
    if(gainWord == LTC6915_GAIN_0){val = 0;}
    else {val = 1 << (gainWord - 1);}
  }
  /* Update output */
  *result = val;
  return error;
}


  
/*******************************************************************************
* Function Name: ltc6915_wordFromValue()
********************************************************************************
* \brief
*  Produces the gain value from the gain word
*
* \param gainWord [in]
*  Gain word to test 
*
* \param result [out]
*  Value of the word
*
* \return
*  Error of the operation
*******************************************************************************/
  uint32_t ltc6915_wordFromValue(uint16_t gainValue, LTC6915_GAIN_T *const result){
  uint32_t error = 0;
  error |= ltc6915_isValidGainVal(gainValue);
  LTC6915_GAIN_T word = 0;
  if(!error){
    for(uint8_t i=0; i<8; i++){
      if(gainValue){
        bool isBitOn = (gainValue >> i) & 0x01;
        if(isBitOn){
          word = i;
          break;
        }
      }
    }
  }
  /* Update output */
  *result = word;
  return error;
}

/*******************************************************************************
* Function Name: ltc6915_isValidGainWord()
********************************************************************************
* \brief
*  Verifies if the gain word [0-13] is a valid LTC6915_GAIN_T
*
* \param gainWord [in]
*  Gain word to test 
*
* \return
*  Result of the operation
*******************************************************************************/
inline bool ltc6915_isValidGainWord(uint8_t gainWord){
  return (gainWord <= LTC6915_GAIN_4096);
}

  /*******************************************************************************
* Function Name: ltc6915_isValidGainVal()
********************************************************************************
* \brief
*  Verifies if the gain value [0-4096] is a valid value represented by a gain word
*
* \param gainVal [in]
* Gain value to check
*
* \return
*  Result of the operation
*******************************************************************************/
bool ltc6915_isValidGainVal(uint8_t gainValue){
  bool isValidGainVal = false;
  for(uint8_t i=0; i<8; i++){
    if(gainValue){
      bool isBitOn = (gainValue >> i) & 0x01;
      if(isBitOn){
        if(gainValue & ~(1 << i)){
          isValidGainVal = true;
        }
        break;
      }
    }
  }
  return isValidGainVal;
}

/*******************************************************************************
* Function Name: ltc6915_getNextGainWord()
********************************************************************************
* \brief
*  Returns the next gain word. If the gain is max (0x0D -> 4096) then overflows to 
*   the min gain (0x00 -> 0)
*   Does not modify the current state.
* \param state [in]
* Gain value to check
*
* \return
*  Next gainWord
*******************************************************************************/
uint32_t ltc6915_getNextGainWord(LTC6915_S *const state, LTC6915_GAIN_T *const nextWord){
  uint32_t error = 0;
  LTC6915_GAIN_T currentGainWord = state->gainWord;
  if(!ltc6915_isValidGainWord(currentGainWord)){error|=ERROR_VAL;}
  if(!error){
    LTC6915_GAIN_T nextGainWord = LTC6915_GAIN_0;
    if(currentGainWord != LTC6915_GAIN_4096){nextGainWord=currentGainWord+1;}
    *nextWord = nextGainWord;
  }
  return error;
}

/*******************************************************************************
* Function Name: ltc6915_getPreviousGainWord()
********************************************************************************
* \brief
*  Returns the previous gain word. If the gain is min (0x00 -> 0) then underflows to
*  the max gainword (0x0D -> 4096)
*   Does not modify the current state.
* \param state [in]
* Gain value to check
*
* \return
*  Next gainWord
*******************************************************************************/
uint32_t ltc6915_getPreviousGainWord(LTC6915_S *const state, LTC6915_GAIN_T *const prevWord){
  uint32_t error = 0;
  LTC6915_GAIN_T currentGainWord = state->gainWord;
  if(!ltc6915_isValidGainWord(currentGainWord)){error|=ERROR_VAL;}
  if(!error){
    LTC6915_GAIN_T prevGainWord = LTC6915_GAIN_4096;
    if(currentGainWord != LTC6915_GAIN_0){prevGainWord=currentGainWord-1;}
    *prevWord = prevGainWord;
  }
  return error;
}


/* [] END OF FILE */