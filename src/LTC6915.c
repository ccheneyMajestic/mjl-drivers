/***************************************************************************
*                                       MICA
* File: LTC6915.c
* Workspace: micaOS
* Project Name: libMica
* Version: v1.0
* Author: Craig Cheney
*
* Brief:
*   Driver file for the LTC6915 Instrumentation amplifier
* 
* Authors:
*   Craig Cheney
*
* Change Log:
*   2020.06.19 CC - Document created
********************************************************************************/
#include "LTC6915.h" 

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
uint32_t ltc6915_init(LTC6915_S *const state, LTC6915_cfg_S *const cfg){
  uint32_t error = 0;
  /* Verify Communication Functions */
  if (LTC6915_MODE_SERIAL == cfg->mode){
    /* Check SPI functions */
    error |= (cfg->fn_spiWriteArray == NULL) ? ERROR_POINTER : ERROR_NONE;
    /* slaveID: No limitation on value */
  }
  /* Parallel control */
  else if(LTC6915_MODE_PARALLEL == cfg->mode){
    error |= (cfg->fn_pin_D0_Write == NULL) ? ERROR_POINTER : ERROR_NONE;
    error |= (cfg->fn_pin_D1_Write == NULL) ? ERROR_POINTER : ERROR_NONE;
    error |= (cfg->fn_pin_D2_Write == NULL) ? ERROR_POINTER : ERROR_NONE;
    error |= (cfg->fn_pin_D3_Write == NULL) ? ERROR_POINTER : ERROR_NONE;
  }
  /* Unknown mode */
  else {error |= ERROR_MODE;}
  /* Check the default gain */
  error |= ltc6915_isValidGainWord(cfg->gainWord) ? ERROR_VAL : ERROR_NONE;
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
  }
  return error;
}

/*******************************************************************************
* Function Name: ltc6915_start()
********************************************************************************
* \brief
*  
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
uint32_t ltc6915_start(LTC6915_S *const state) {
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}

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
    error |= state->_init ? 0 : ERROR_INIT;
    error |= ltc6915_isValidGainWord(gainWord) ? ERROR_VAL: 0;

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
  error |= ltc6915_isValidGainWord(gainWord);
  uint8_t val = 0;
  if(!error){
    if(gainWord == LTC6915_GAIN_0){val = 0;}
    else {val = 1 << (gainWord - 1);}
  }
  /* Update output */
  *result = val;
  return error;
}


  
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
bool ltc6915_isValidGainWord(uint8_t gainWord){
  bool isValidGainWord = false;
  for(uint8_t i=LTC6915_GAIN_0; i<LTC6915_GAIN_4096; i++){
    if(gainWord == i){
      isValidGainWord == true;
      break;
    }
  }
  return isValidGainWord;
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
/* [] END OF FILE */