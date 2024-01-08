/***************************************************************************
*                              Majestic Labs  © 2024
*
* File: max31856.c
* Workspace: mjl_drivers
* Version: 1.0.0
* Authors: C. Cheney
*
* Brief:
*   Driver header for max31856.c thermocouple amplifier 
*
* 2019.06.20 - Document Created by CC
* 2024.01.07 - Ported into MJL Drivers
********************************************************************************/
#include "max31856.h"
#include "mjl_errors.h"

/* Default configuration struct */
const max31856_cfg_s max31856_config_default  = {
  .spi = NULL,
  .slaveId = 0,
};

/*******************************************************************************
* Function Name: max31856_init()
********************************************************************************
* \brief
*   Initialize the state structure with the configurations provided  
*
* \param state [in/out]
*   Pointer to the state struct
*
* \return
*   Error code of the operation
*******************************************************************************/
uint32_t max31856_init(max31856_state_s *const state, max31856_cfg_s *const cfg){
  uint32_t error = 0;
  /* Set state params */
  state->_init = false;
  state->_running = false;
  /* Verify required functions */
  error |= (NULL == cfg->spi) ? ERROR_POINTER : ERROR_NONE;
  
  if(!error) {
    /* Copy config params */
    state->spi = cfg->spi;
    state->slaveId = cfg->slaveId;
    /* Mark as initialized */
    state->_init = true;    
  }
  return error;
}


/*******************************************************************************
* Function Name: max31856_start()
********************************************************************************
* \brief
*   Sets the device to Automatic conversion mode, four sample average, type K  
*
* \param state [in/out]
*   Pointer to the state struct
*
* \return
*   Error code of the operation
*******************************************************************************/
uint32_t max31856_start(max31856_state_s *const state){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(!state->spi->_init){error|=ERROR_INIT;}
  if(!state->spi->_running){error|=ERROR_MODE;}

  if(!error){
    /* Pre-mark as running */
    state->_running = true;
    /* Write to the configuration registers */
    error |= max31856_writeReg(state, MAX31856_CR0_ADDR, MAX31856_CR0_MASK_CMODE);
    uint8_t CR1_val = (MAX31856_CR1_MASK_TC_K | MAX31856_CR1_MASK_AVG4);
    error |= max31856_writeReg(state, MAX31856_CR1_ADDR, CR1_val);
    /* Read back the config register */
    uint8_t readVal = 0;
    error |= max31856_readReg(state, MAX31856_CR1_ADDR, &readVal);
    if(error || (readVal != CR1_val)) {
      error = ERROR_VAL;
      state->error = readVal;    
    } 
  }

  if(error) {state->_running = false;}
  return error;
}

/*******************************************************************************
* Function Name: max31856_readReg()
********************************************************************************
* \brief
*   Reads a register from the MAX31856 
*
* \param state [in/out]
*   Pointer to the state struct
*
* \param addr [in]
*   Address of the register
*
* \param result [out]
*   Return value of the register 
*  
*
* \return
*   Error code of the operation
*******************************************************************************/
uint32_t max31856_readReg(max31856_state_s* state, uint8_t addr, uint8_t *result){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(!state->_running){error|= ERROR_STOPPED;}

  uint8_t data[2] = {0x00};
  data[0] = addr;
  if(!error) {
    error |= spi_readArray(state->spi, state->slaveId, data, 2);
    *result = data[1];

  }
  return error;
}

/*******************************************************************************
* Function Name: max31856_writeReg()
********************************************************************************
* \brief
*   Writes to a register on the MAX31856 
*
* \param state [in/out]
*   Pointer to the state struct
*
* \param addr [in]
*   Address of the register
*
* \param val [in]
*   Value of the register 
*  
*
* \return
*   Error code of the operation
*******************************************************************************/
uint32_t max31856_writeReg(max31856_state_s* state, uint8_t addr, uint8_t val){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(!state->_running){error|= ERROR_STOPPED;}

  if(!error) {
    /* Set slave active */
    #define LEN_WRITE_REG  (2)
    uint8_t data[LEN_WRITE_REG] = {(addr|MAX31856_MASK_WRITEADDR), val};
    error |= spi_writeArray_blocking(state->spi, state->slaveId, data, LEN_WRITE_REG);
  }
  return error;
}

/*******************************************************************************
* Function Name: max31856_readArray()
********************************************************************************
* \brief
*   Reads multiple register from the MAX31856 
*
* \param state [in/out]
*   Pointer to the state struct
*
* \param addr [in]
*   Address of the register
*
* \param val [out]
*   Return value of the register 
*  
*
* \return
*   Error code of the operation
*******************************************************************************/
uint32_t max31856_readArray(max31856_state_s* state, uint8_t *array, uint8_t len){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(!state->_running){error|= ERROR_STOPPED;}

  if(!error) {
    error |= spi_readArray(state->spi, state->slaveId, array, len);

  }
  return error;
}

/*******************************************************************************
* Function Name: max31856_writeArray()
********************************************************************************
* \brief
*   Writes to multiple register from the MAX31856 
*
* \param state [in/out]
*   Pointer to the state struct
*
* \param addr [in]
*   Start Address of the registers
*
* \param val [out]
*   Values to write
*
* \return
*   Error code of the operation
*******************************************************************************/
uint32_t max31856_writeArray(max31856_state_s* state, uint8_t *array, uint8_t len){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(!state->_running){error|= ERROR_STOPPED;}

  if(!error) {
    error |= spi_writeArray_blocking(state->spi, state->slaveId, array, len);

  }
  return error;
}


/*******************************************************************************
* Function Name: max31856_readTemp()
********************************************************************************
* \brief
*   Executes a read command from the device. Places the result into the
*   'temp' field of the state struct. 
*
* \param state [in/out]
*   Pointer to the state struct
*
* \return
*   Error code of the operation
*******************************************************************************/
uint32_t max31856_readTemp(max31856_state_s* state){
  #define LEN_READ_TEMP  (4)
  uint8_t tempArray[LEN_READ_TEMP] = {0x00};
  tempArray[0] = MAX31856_LTCBH_ADDR;
  /* SPI Read data */
  uint32_t error = max31856_readArray(state, tempArray, LEN_READ_TEMP);
  if(!error) {
    uint32_t tempBin = 
      (tempArray[1] << 16) | 
      (tempArray[2]  << 8)  |
      (tempArray[3]);
    /* Convert to a float */
    state->temp = max31856_convertTemp(tempBin);
  }
  return error;
}

/*******************************************************************************
* Function Name: max31856_ConvertTemp()
********************************************************************************
* \brief
*   Converts a left justified binary temp to a float (°C)
*
* \param state [in]
*   Temperature in binary
*
* \return
*   Temperature in °C
*******************************************************************************/
float max31856_convertTemp(uint32_t temp){
  uint8_t i;
  float tempF = 0.0;
  int8_t power = MAX31856_TEMP_START_POWER;
  for(i = MAX31856_TEMP_SHIFT_LSB; i <= MAX31856_TEMP_SHIFT_MSB; i++){
    if(temp & (1 << i)){
      /* Avoid needing math.h */
      // tempF += powf(TWO_F, (float) power);
      tempF += power >= 0 ? (float) (1 << power) : (1 / (float) (1<< (-1*power)));
    }
    power++;
  }
  /* Sign Bit */
  if(temp & MAX31856_TEMP_MASK_SIGN){tempF = tempF - MAX31856_TEMP_NEG;}
  return tempF;
}


/* [] END OF FILE */
