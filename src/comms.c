/***************************************************************************
*                               Majestic Labs © 2023
* File: comms.c
* Workspace: MJL Driver Library 
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: Communication utility functions  
*   
*
* 2023.04.25  - Document Created
********************************************************************************/
#include "comms.h"
#include <stddef.h>

/*******************************************************************************
* Function Name: Comms_validateUart()
****************************************************************************//**
* \brief
*  Validate the UART communication structure provided
*
* \param i2c
*  Pointer to the UART Struct
*
* \return
*  An error code with the result
*******************************************************************************/
uint32_t Comms_validateUart(COMMS_UART_S *uart){
  uint32_t error = COMMS_ERROR_NONE;
  error |= (uart == NULL)               ? COMMS_ERROR_START : COMMS_ERROR_NONE;
  error |= (uart->write == NULL)       ? COMMS_ERROR_WRITE : COMMS_ERROR_NONE;
  error |= (uart->print == NULL) ? COMMS_ERROR_WRITE_ARRAY : COMMS_ERROR_NONE;
  error |= (uart->writeArray == NULL)  ? COMMS_ERROR_WRITE_ARRAY : COMMS_ERROR_NONE;
  error |= (uart->read == NULL)        ? COMMS_ERROR_READ : COMMS_ERROR_NONE;
  error |= (uart->getRxBufferSize == NULL)   ? COMMS_ERROR_RXBUFFER : COMMS_ERROR_NONE;
  error |= (uart->getTxBufferSize == NULL)   ? COMMS_ERROR_TXBUFFER : COMMS_ERROR_NONE;
  return error;
}



/*******************************************************************************
* Function Name: Comms_validateI2C()
****************************************************************************//**
* \brief
*  Validate the I2C communication structure provided
*
* \param i2c
*  Pointer to the I2C Struct
*
* \return
*  An error code with the result
*******************************************************************************/
uint32_t Comms_validateI2C(COMMS_I2C_S *i2c){
  uint32_t error = COMMS_ERROR_NONE;
  error |= (i2c == NULL)              ? COMMS_ERROR_I2C : COMMS_ERROR_NONE; 
  error |= (i2c->write == NULL)       ? COMMS_ERROR_WRITE : COMMS_ERROR_NONE;
  error |= (i2c->writeCmd == NULL)    ? COMMS_ERROR_WRITE_CMD : COMMS_ERROR_NONE;
  error |= (i2c->writeArray == NULL)  ? COMMS_ERROR_WRITE_ARRAY : COMMS_ERROR_NONE;
  error |= (i2c->read == NULL)        ? COMMS_ERROR_READ : COMMS_ERROR_NONE;
  error |= (i2c->readCmd == NULL)     ? COMMS_ERROR_READ_CMD : COMMS_ERROR_NONE;
  error |= (i2c->readArray == NULL)   ? COMMS_ERROR_READ_ARRAY : COMMS_ERROR_NONE;
  return error;
}

/*******************************************************************************
* Function Name: Comms_validateSpi()
****************************************************************************//**
* \brief
*  Validate the UART communication structure provided
*
* \param i2c
*  Pointer to the UART Struct
*
* \return
*  An error code with the result
*******************************************************************************/
uint32_t Comms_validateSpi(COMMS_SPI_S *spi) {
    uint32_t error = COMMS_ERROR_NONE;
    error |= (spi == NULL)              ? COMMS_ERROR_SPI : COMMS_ERROR_NONE;
    error |= (spi->setActive == NULL)   ? COMMS_ERROR_SPI : COMMS_ERROR_NONE;
    error |= (spi->write == NULL)       ? COMMS_ERROR_WRITE : COMMS_ERROR_NONE;
    error |= (spi->writeArray == NULL)  ? COMMS_ERROR_WRITE_ARRAY : COMMS_ERROR_NONE;
    error |= (spi->read == NULL)        ? COMMS_ERROR_READ : COMMS_ERROR_NONE;
    error |= (spi->getRxBufferSize == NULL)   ? COMMS_ERROR_RXBUFFER : COMMS_ERROR_NONE;
    error |= (spi->getTxBufferSize == NULL)   ? COMMS_ERROR_TXBUFFER : COMMS_ERROR_NONE;
    error |= (spi->clearRxBuffer == NULL) ? COMMS_ERROR_RXBUFFER : COMMS_ERROR_NONE;
    error |= (spi->clearTxBuffer == NULL) ? COMMS_ERROR_TXBUFFER : COMMS_ERROR_NONE;
    
    return error;
}

/*******************************************************************************
* Function Name: float2Byte()
****************************************************************************//**
* \brief
*  Converts a float to a byte array
*
* \param fIn [In]
*  Input float
*
* \param array [out]
* Output array. Must be at lease 4 bytes long
*
* \return
*  none
*******************************************************************************/
void float2Byte(float fIn, uint8_t* array) {
  floatByte_U data;
  data.num = fIn;
  uint8_t i;
  for(i=0; i<BYTES_PER_FLOAT; i++) {
    array[i] = data.bytes[i];
  }
}

/*******************************************************************************
* Function Name: byte2Float()
****************************************************************************//**
* \brief
*  Converts a byte array to a float 
*
* \param array [in]
* Input array. Must be at lease 4 bytes long
*
* \param fOut [out]
*  Resulting float
*
* \return
*  none
*******************************************************************************/
void byte2Float(uint8_t* array, float* fOut) {
  floatByte_U data;
  uint8_t i;
  for(i=0; i<BYTES_PER_FLOAT; i++){
    data.bytes[i] = array[i];
  }
  *fOut = data.num;
}

/*******************************************************************************
* Function Name: uint32ToByte()
****************************************************************************//**
* \brief
*  Converts a uin32_t to an array
*
* \param in [in]
*  Uint32 input
*
* \param array [out]
*  Output array. Must be at lease 4 bytes long
*
* \return
*  none
*******************************************************************************/
void uint32ToByte(uint32_t in, uint8_t* array) {
  uint32Byte_U data;
  data.num = in;
  uint8_t i;
  for(i=0; i<BYTES_PER_UINT32; i++) {
    array[i] = data.bytes[i];
  }
}

/*******************************************************************************
* Function Name: uint16ToByte()
****************************************************************************//**
* \brief
*  Converts a uint16_t to an array
*
* \param in [in]
*  Uint16 input
*
* \param array [out]
*  Output array. Must be at lease 2 bytes long
*
* \return
*  none
*******************************************************************************/
void uint16ToByte(uint16_t in, uint8_t* array) {
  uint16Byte_U data;
  data.num = in;
  uint8_t i;
  for(i=0; i<BYTES_PER_UINT16; i++) {
    array[i] = data.bytes[i];
  }
}

/*******************************************************************************
* Function Name: ByteToUint16()
****************************************************************************//**
* \brief
*  Converts a byte array uint16_t
*
* \param array [in]
*  Input array. Must be at lease 2 bytes long
*
* \param out [out]
*  Output uint16_t
*
* \return
*  none
*******************************************************************************/
void ByteToUint16(uint8_t* array, uint16_t* out) {
  uint16Byte_U data;
  uint8_t i;
  for(i=0; i<BYTES_PER_UINT16; i++){
    data.bytes[i] = array[i];
  }
  *out = data.num;
}



/*******************************************************************************
* Function Name: int16ToByte()
****************************************************************************//**
* \brief
*  Converts a signed int16_t to an array
*
* \param in [in]
*  Uint16 input
*
* \param array [out]
*  Output array. Must be at lease 2 bytes long
*
* \return
*  none
*******************************************************************************/
void int16ToByte(int16_t in, uint8_t* array) {
  int16Byte_U data;
  data.num = in;
  uint8_t i;
  for(i=0; i<BYTES_PER_UINT16; i++) {
    array[i] = data.bytes[i];
  }
}

/*******************************************************************************
* Function Name: ByteToInt16()
****************************************************************************//**
* \brief
*  Converts an array to a uint16_t
*
* \param array [in]
*  Array input. Must be at lease 2 bytes long
*
* \param out [out]
*  Output int16_t
*
* \return
*  none
*******************************************************************************/
void ByteToInt16(uint8_t* array, int16_t* out) {
  int16Byte_U data;
  uint8_t i;
  for(i=0; i<BYTES_PER_INT16; i++){
    data.bytes[i] = array[i];
  }
  *out = data.num;
}

/*******************************************************************************
* Function Name: int16ToFloat()
****************************************************************************//**
* \brief
*  Converts a int16_t to a float
*
* \param in [in]
*  Int16 input
*
* \param out [out]
*  Output float
*
* \return
*  none
*******************************************************************************/
void int16ToFloat(int16_t in, float* out) {
  int16Float_U data;
  data.num = in;
  *out = data.numF;
}

/*******************************************************************************
* Function Name: ByteToUint32()
****************************************************************************//**
* \brief
*  Converts a byte array to a uint32_t 
*
* \param array [in]
* Input array. Must be at least 4 bytes long
*
* \param out [out]
*  Resulting uint32_t
*
* \return
*  none
*******************************************************************************/
void ByteToUint32(uint8_t* array, uint32_t* out){  
  uint32Byte_U data;
  uint8_t i;
  for(i=0; i<BYTES_PER_UINT32; i++){
    data.bytes[i] = array[i];
  }
  *out = data.num;
}



/* [] END OF FILE */
