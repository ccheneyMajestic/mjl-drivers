/***************************************************************************
*                                Majestic Labs © 2023
* File: mjl_spi.h
* Workspace: MJL Driver Library 
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: Serial Peripheral Interface middleware driver 
*   
*
* 2024.01.07 - Document Created
********************************************************************************/
#include "mjl_spi.h"
#include "mjl_errors.h"

const MJL_SPI_CFG_S spi_cfg_default = {
  .req_hal_writeArray_blocking = NULL,
  .req_hal_read = NULL,
  .req_hal_setActive = NULL,
  .req_hal_getRxBufferNum = NULL,
  .req_hal_getTxBufferNum = NULL,
  .req_hal_clearRxBuffer = NULL,
  .req_hal_clearTxBuffer = NULL,
  .opt_hal_externalStart = NULL,
  .opt_hal_externalStop = NULL,
};

/*******************************************************************************
* Function Name: spi_init()
********************************************************************************
* \brief
*   Initializes the SPI state struct from a configuration struct 
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
uint32_t spi_init(MJL_SPI_S *const state, MJL_SPI_CFG_S *const cfg){
  uint32_t error = 0;
  /* Verify required functions */
  error |= (NULL == cfg->req_hal_writeArray_blocking) ? ERROR_POINTER : ERROR_NONE;
  error |= (NULL == cfg->req_hal_read) ? ERROR_POINTER : ERROR_NONE;
  error |= (NULL == cfg->req_hal_setActive) ? ERROR_POINTER : ERROR_NONE;
  error |= (NULL == cfg->req_hal_getRxBufferNum) ? ERROR_POINTER : ERROR_NONE;
  error |= (NULL == cfg->req_hal_getTxBufferNum) ? ERROR_POINTER : ERROR_NONE;
  error |= (NULL == cfg->req_hal_clearRxBuffer) ? ERROR_POINTER : ERROR_NONE;
  error |= (NULL == cfg->req_hal_clearTxBuffer) ? ERROR_POINTER : ERROR_NONE;
  error |= (NULL == cfg->opt_hal_externalStart) ? ERROR_POINTER : ERROR_NONE;
  error |= (NULL == cfg->opt_hal_externalStop) ? ERROR_POINTER : ERROR_NONE;
  /* Valid Inputs */
  if(!error) {
    /* Copy params */
    state->req_hal_writeArray_blocking = cfg->req_hal_writeArray_blocking; 
    state->req_hal_read = cfg->req_hal_read; 
    state->req_hal_setActive = cfg->req_hal_setActive; 
    state->req_hal_getRxBufferNum = cfg->req_hal_getRxBufferNum; 
    state->req_hal_getTxBufferNum = cfg->req_hal_getTxBufferNum; 
    state->req_hal_clearRxBuffer = cfg->req_hal_clearRxBuffer; 
    state->req_hal_clearTxBuffer = cfg->req_hal_clearTxBuffer; 
    state->opt_hal_externalStart = cfg->opt_hal_externalStart; 
    state->opt_hal_externalStop = cfg->opt_hal_externalStop; 
    /* Mark as initialized */
    state->_init = true;
    state->_running = false;
  }
  if(error){state->_init=false;}
  return error;
}

/*******************************************************************************
* Function Name: spi_start()
********************************************************************************
* \brief
*   Starts the SPI and call the external start function, if present 
*
* \param state [in/out]
* Pointer to the state struct
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t spi_start(MJL_SPI_S *const state){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(state->_running){error|=ERROR_RUNNING;}

  if(!error){
    /* Pre-mark as running */
    state->_running = true;
    /* Run the external start function if present  */
    if(NULL != state->opt_hal_externalStart){
      error |= state->opt_hal_externalStart((MJL_SPI_T *const) state);
    }
  }
  return error;
}

/*******************************************************************************
* Function Name: spi_stop()
********************************************************************************
* \brief
*   Starts the SPI and call the external start function, if present 
*
* \param state [in/out]
* Pointer to the state struct
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t spi_stop(MJL_SPI_S *const state){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(!state->_running){error|=ERROR_STOPPED;}

  if(!error){
    /* Run the external start function if present  */
    if(NULL != state->opt_hal_externalStop){
      error |= state->opt_hal_externalStop((MJL_SPI_T *const) state);
    }
    state->_running = false;
  }
  return error;
}

/*******************************************************************************
* Function Name: spi_writeArray_blocking()
********************************************************************************
* \brief
*   Write an array of data out via the SPI port
*
* \param state [in/out]
* Pointer to the state struct
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t spi_writeArray_blocking(MJL_SPI_S *const state, uint8_t id, uint8_t * array, uint16_t len){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(!state->_running){error|=ERROR_STOPPED;}

  if(!error){
    /* Select the slave */
    error|= state->req_hal_setActive(id); 
    /* Place the data in the TX buffer*/
    error|= state->req_hal_writeArray_blocking(array, len);
    /* Wait until TX buffer is clear */    
    uint32_t txNum = len;
    do{
      txNum = state->req_hal_getTxBufferNum();
      // TODO: Add timeout
    }
    while(txNum > 0);
    /* Wait for RX buffer to populate */
    uint32_t rxNum = 0;
    do{
      rxNum = state->req_hal_getRxBufferNum();
      // TODO: Add timeout
    }
    while(rxNum < len);
    /* Clear the dummy data in the RX Buffer */
    error|= state->req_hal_clearRxBuffer();
  }
  return error;
}

/*******************************************************************************
* Function Name: spi_readArray()
********************************************************************************
* \brief
*   Read data from the SPI port
*
* \param state [in/out]
* Pointer to the state struct
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t spi_readArray(MJL_SPI_S *const state,  uint8_t id, uint8_t * array, uint16_t len){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(!state->_running){error|=ERROR_STOPPED;}

  if(!error){
    /* Clear the receive buffer */
    error|= state->req_hal_clearRxBuffer();
    /* Select the slave */
    error|= state->req_hal_setActive(id); 
    /* Write out the number of bytes wish to be read - These can be dummy bytes (0), or command bytes */
    error|= state->req_hal_writeArray_blocking(array, len);
    /* Place the data from the RX Buffer into the results buffer */
    uint32_t val = 0;
    do{
      val = state->req_hal_getRxBufferNum();
      // TODO: add timeout
    }
    while(val < len);
    /* Retrieve the data from the buffer */
    for(uint16_t i=0; i < len; i++){
      error|= state->req_hal_read(&array[i]);
    }
  }
  return error;
}

/*******************************************************************************
* Function Name: spi_writeArray()
********************************************************************************
* \brief
*   Write an array of data out via the SPI port
*
* \param state [in/out]
* Pointer to the state struct
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t spi_write(MJL_SPI_S *const state, uint8_t id, uint8_t data){
  return spi_writeArray_blocking(state, id, &data, 1);
}

/*******************************************************************************
* Function Name: spi_writeArray()
********************************************************************************
* \brief
*   Write an array of data out via the SPI port
*
* \param state [in/out]
* Pointer to the state struct
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t spi_read(MJL_SPI_S *const state,  uint8_t id, uint8_t * data){
  return spi_readArray(state, id, data, 1);
}

/* [] END OF FILE */