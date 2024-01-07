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
  .req_hal_writeArray = NULL,
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
* Function Name: uart_init()
********************************************************************************
* \brief
*   Initializes the uart state struct from a configuration struct 
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
  error |= (NULL == cfg->req_hal_writeArray) ? ERROR_POINTER : ERROR_NONE;
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
    state->req_hal_writeArray = cfg->req_hal_writeArray; 
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
    state->isLoggingEnabled = true;
  }
  if(error){state->_init=false;}
  return error;
}



/* [] END OF FILE */


// /***************************************************************************
// *                              Impact Biosystems © 2021
// *                               
// *
// * File: PSoC6_SPI.c
// * Workspace: 05_Gen2
// * Project: scannerDriver_v4.0
// * Version: 1.0.0
// * Authors: E. Burba
// * 
// * PCB: Scanner Driver v4.0.0
// * mcuType: PSoC 6 BLE Module
// *
// * Brief:
// *   SPI for PSoC6
// *
// * 2021.08.03 - EB Document Created
// ********************************************************************************/
// #include "PSoC6_SPI.h"
// #include "project.h"

// /*******************************************************************************
// * Function Name: spiPsoc6_start()
// ****************************************************************************//**
// * \brief
// *  PSoC6 to generic SPI Start wrapper
// *   Starts the SPI bus
// *
// * \param spi
// *   Pointer to the generic COMS_SPI_S to regster the functions
// *  
// *
// * \return
// *  An error code with the result of the start feature 
// *******************************************************************************/
// uint32_t spiPsoc6_start(COMMS_SPI_S *spi) {
//     spi->setActive = spiPsoc6_setActive;
//     spi->write = spiPsoc6_write;
//     spi->read = spiPsoc6_read;   
//     spi->writeArray = spiPsoc6_writeArray;   
//     spi->readArray = spiPsoc6_readArray;
//     spi->getRxBufferSize = spiPsoc6_getRxBufferSize;
//     spi->getTxBufferSize = spiPsoc6_getTxBufferSize;
//     spi->clearRxBuffer = spiPsoc6_clearRxBuffer;
//     spi->clearTxBuffer = spiPsoc6_clearTxBuffer;
//     /* Start the component */
//     SPI_Start();
//     spiPsoc6_clearRxBuffer();
//     spiPsoc6_clearTxBuffer();
    
//     return COMMS_ERROR_NONE;
// }


// /*******************************************************************************
// * Function Name: spiPsoc6_setActive()
// ****************************************************************************//**
// * \brief
// *  PSoC6 to generic SPI set active reg wrapper
// *   Writes the pin low of the desired device
// *
// * \param addr [in]
// *   Register Address
// *
// * \param val [in]
// *   Value to write to the register
// *  
// *
// * \return
// *  An error code with the result of the write feature 
// *******************************************************************************/
// uint32_t spiPsoc6_setActive(uint8_t id) {
//     uint32_t error = COMMS_ERROR_NONE;
//     SPI_SetActiveSlaveSelect(id);
//     return error;
// }

// /*******************************************************************************
// * Function Name: spiPsoc6_write()
// ****************************************************************************//**
// * \brief
// *  PSoC to generic SPI write reg wrapper
// *   Writes a value to a specific register. This is currently a blocking function
// *
// * \param addr [in]
// *   Register Address
// *
// * \param val [in]
// *   Value to write to the register
// *  
// *
// * \return
// *  An error code with the result of the write feature 
// *******************************************************************************/
// uint32_t spiPsoc6_write(uint8_t addr, uint8_t val){
//     uint32_t error = COMMS_ERROR_NONE;
//     /* Ensure RX queue is empty */
//     if (SPI_GetNumInRxFifo() == ZERO){
//         SPI_Write(addr);
//         SPI_Write(val);
//         /* Wait until the data has been sent */
//         while(SPI_GetNumInTxFifo()){}
//         /* Wait for the data to be received */
//         while(SPI_GetNumInRxFifo() != TWO) {}
//         /* Throw away dummy bytes */
//         SPI_ClearRxFifo();
//     } else {
//         error = COMMS_ERROR_RXBUFFER;
//     }
//     /* Return Error  */
//     return error;
// }

// /*******************************************************************************
// * Function Name: spiPsoc6_read()
// ****************************************************************************//**
// * \brief
// *  PSoC6 to generic SPI read reg wrapper
// *   Reads a value to a specific register - Assumes a software Buffer
// *   Currently this a blocking function. 
// *
// * \param addr [in]
// *   Register Address
// *
// * \param ret [out]
// *   Pointer to return value 
// *  
// *
// * \return
// *  An error code with the result of the read feature 
// *******************************************************************************/
// uint32_t spiPsoc6_read(uint8_t addr, uint8_t *ret){
//     uint32_t error = COMMS_ERROR_NONE;
//     /* Ensure RX queue is empty */
//     if (SPI_GetNumInRxFifo() == ZERO){
//         /* Need to pulse the clock with empty byte */
//         uint8_t array[TWO] = {addr, ZERO};
//         SPI_WriteArray(array, TWO);
//         /* Wait until the data has been sent */
//         while(SPI_GetNumInTxFifo()){}
//         /* Wait for the data to be received */
//         while(SPI_GetNumInRxFifo() != TWO) {}
//         /* Throw away dummy byte */
//         SPI_Read();
//         *ret = SPI_Read();  
//     } else {
//         error = COMMS_ERROR_RXBUFFER;
//     }
//     /* Return the error code */
//     return error;
// }


// /*******************************************************************************
// * Function Name: spiPsoc6_writeArray()
// ****************************************************************************//**
// * \brief
// *  PSoC6 to generic SPI Write Aray
// *   Writes an array of data out of the SPI port
// *
// * \param addr [in]
// *   Register Address
// *
// * \param array [in]
// *   Pointer array containing data elements
// *
// * \param len [in]
//     Length of the data to transmit (excluding addr byte)
// *
// * \return
// *  An error code with the result of the command
// *******************************************************************************/
// uint32_t spiPsoc6_writeArray(uint8_t addr, uint8_t* array, uint16_t len){
//     uint32_t error = COMMS_ERROR_NONE;
//     /* Ensure RX queue is empty */
//     if( SPI_GetNumInRxFifo() == ZERO) {
//         SPI_Write(addr);
//         SPI_WriteArray(array, len);
//         /* Wait until the data has been sent */
//         while(SPI_GetNumInTxFifo()){}
//         /* Wait for the dummy data to be received */
//         while(SPI_GetNumInTxFifo() != (len + ONE)) {}
//         /* Throw away dummy bytes */
//         SPI_ClearRxFifo();
//     } else {
//         error = COMMS_ERROR_RXBUFFER;
//     }
//     return error;
// }


// /*******************************************************************************
// * Function Name: spiPsoc6_readArray()
// ****************************************************************************//**
// * \brief
// *  PSoC6 to generic SPI Read Aray
// *   Writes an array of data out of the SPI port
// *
// * \param addr [in]
// *   Register Address
// *
// * \param array [out]
// *   Pointer array to return data elements. Must be >= len in size
// *
// * \param len [in]
//     Number of bytes to read (excluding addr byte)
// *
// * \return
// *  An error code with the result of the command
// *******************************************************************************/
// uint32_t spiPsoc6_readArray(uint8_t addr, uint8_t* array, uint16_t len){
//     uint32_t error = COMMS_ERROR_NONE;
//     /* Ensure RX queue is empty */
//     if( SPI_GetNumInRxFifo() == ZERO) {
//         /* Write out the address byte */
//         SPI_Write(addr);
//         uint16_t i;
//         /* Read in the data */
//         for(i=ZERO; i < len; i++){
//             SPI_Write(ZERO);   
//         }
//         /* Wait until the data has been sent */
//         while(SPI_GetNumInTxFifo()){}
//         /* Wait for the dummy data to be received */
//         while(SPI_GetNumInRxFifo() != (len + ONE)) {}
//         /* Throw away dummy byte (Addr) */
//         SPI_Read();
//         /* Transmit out data */
//         for(i=ZERO; i < len; i++){
//             array[i] = SPI_Read(); 
//         }
//     } else {
//         error = COMMS_ERROR_RXBUFFER;
//     }
//     return error;
// }

// /*******************************************************************************
// * Function Name: spiPsoc6_getRxBufferSize()
// ****************************************************************************//**
// * \brief
// *  PSoC6 to generic SPI
// *   Returns the size of the RX buffer
// *
// * \param result [out]
// *   Result of the operation
// *
// * \return
// *  Size of the RX buffer
// *******************************************************************************/
// uint32_t spiPsoc6_getRxBufferSize(uint8_t *result){
//     *result = SPI_GetNumInRxFifo();
//     return COMMS_ERROR_NONE;  
// }

// /*******************************************************************************
// * Function Name: spiPsoc6_getTxBufferSize()
// ****************************************************************************//**
// * \brief
// *  PSoC6 to generic SPI
// *   Returns the size of the TX buffer
// *
// * \param result [out]
// *   Result of the operation
// *
// * \return
// *  Size of the TX buffer
// *******************************************************************************/
// uint32_t spiPsoc6_getTxBufferSize(uint8_t *result){
//     *result = SPI_GetNumInTxFifo();   
//     return COMMS_ERROR_NONE;  
// }

// /*******************************************************************************
// * Function Name: spiPsoc6_clearRxBuffer()
// ****************************************************************************//**
// * \brief
// *  PSoC to generic SPI
// *   Clears the RX buffer
// *
// * \return
// *  None
// *******************************************************************************/
// uint32_t spiPsoc6_clearRxBuffer(void){
//     SPI_ClearRxFifo();   
//     return COMMS_ERROR_NONE;  
// }

// /*******************************************************************************
// * Function Name: spiPsoc6_clearTxBuffer()
// ****************************************************************************//**
// * \brief
// *  PSoC to generic SPI
// *   Clears the TX buffer
// *
// * \return
// *  None
// *******************************************************************************/
// uint32_t spiPsoc6_clearTxBuffer(void){
//     SPI_ClearTxFifo();   
//     return COMMS_ERROR_NONE;  
// }
