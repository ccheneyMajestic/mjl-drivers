/***************************************************************************
*                               Majestic Labs © 2023
* File: comms.h
* Workspace: MJL Driver Library 
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: Communication types including uart, SPI, I2C, etc.  
*   
*
* 2023.04.25  - Document Created
********************************************************************************/
/* Header Guard */
#ifndef COMMS_H
  #define COMMS_H
  /***************************************
  * Included Files
  ***************************************/  
  #include <stdint.h>
  #include <stdbool.h>
  /***************************************
  * Macros
  ***************************************/  
  #define COMMS_ERROR_SHIFT_WRITE             (0) /**< Shift of a null write function */
  #define COMMS_ERROR_SHIFT_WRITE_CMD         (1) /**< Shift of a null write command function */
  #define COMMS_ERROR_SHIFT_WRITE_ARRAY       (2) /**< Shift of a null write array function */
  #define COMMS_ERROR_SHIFT_READ              (3) /**< Shift of a null read function */
  #define COMMS_ERROR_SHIFT_READ_CMD          (4) /**< Shift of a null read cmd function */
  #define COMMS_ERROR_SHIFT_READ_ARRAY        (5) /**< Shift of a null read array function */
  #define COMMS_ERROR_SHIFT_I2C               (6) /**< Shift of the state not present error */
  #define COMMS_ERROR_SHIFT_SPI               (7) /**< Shift of the state not present error */
  #define COMMS_ERROR_SHIFT_UART              (8) /**< Shift of the state not present error */
  #define COMMS_ERROR_SHIFT_START             (9) /**< Shift of an incorrect start procedure */
  #define COMMS_ERROR_SHIFT_RANGE             (10) /**< Shift of out of range error */
  #define COMMS_ERROR_SHIFT_VAL               (11) /**< Shift of out of incorrect value*/
  #define COMMS_ERROR_SHIFT_ADDR              (12) /**< Shift of an invalid address */
  #define COMMS_ERROR_SHIFT_RXBUFFER          (13) /**< Shift of an missing RX buffer */
  #define COMMS_ERROR_SHIFT_TXBUFFER          (14) /**< Shift of an missing TX buffer */
  #define COMMS_ERROR_SHIFT_INTER_CPU         (15) /**< Shift of inter cpu communications error */


  #define COMMS_ERROR_NONE                    (0) /**< No error occurred */
  #define COMMS_ERROR_WRITE                   (1u << COMMS_ERROR_SHIFT_WRITE)       /**< Flag of a null write function */
  #define COMMS_ERROR_WRITE_CMD               (1u << COMMS_ERROR_SHIFT_WRITE_CMD)   /**< Flag of a null write command function */
  #define COMMS_ERROR_WRITE_ARRAY             (1u << COMMS_ERROR_SHIFT_WRITE_ARRAY) /**< Flag of a null write array function */
  #define COMMS_ERROR_READ                    (1u << COMMS_ERROR_SHIFT_READ)        /**< Flag of a null read function */
  #define COMMS_ERROR_READ_CMD                (1u << COMMS_ERROR_SHIFT_READ_CMD) /**< Shift of a null read cmd function */
  #define COMMS_ERROR_READ_ARRAY              (1u << COMMS_ERROR_SHIFT_READ_ARRAY)  /**< Flag of a null read array function */
  #define COMMS_ERROR_I2C                     (1u << COMMS_ERROR_SHIFT_I2C)       /**< Flag of the state not present error */
  #define COMMS_ERROR_SPI                     (1u << COMMS_ERROR_SHIFT_SPI)       /**< Flag of the state not present error */
  #define COMMS_ERROR_UART                    (1u << COMMS_ERROR_SHIFT_UART)       /**< Flag of the state not present error */
  #define COMMS_ERROR_START                   (1u << COMMS_ERROR_SHIFT_START)       /**< Flag of an incorrect start procedure */
  #define COMMS_ERROR_RANGE                   (1u << COMMS_ERROR_SHIFT_RANGE)       /**< Flag of out of range error */
  #define COMMS_ERROR_VAL                     (1u << COMMS_ERROR_SHIFT_VAL)         /**< Flag of out of  incorrect value*/
  #define COMMS_ERROR_ADDR                    (1u << COMMS_ERROR_SHIFT_ADDR)        /**< Flag of an invalid address */
  #define COMMS_ERROR_RXBUFFER                (1u << COMMS_ERROR_SHIFT_RXBUFFER)    /**< Flag of an missing RX buffer */
  #define COMMS_ERROR_TXBUFFER                (1u << COMMS_ERROR_SHIFT_TXBUFFER)    /**< Flag of an missing TX buffer */     
  #define COMMS_ERROR_INTER_CPU                (1u << COMMS_ERROR_SHIFT_INTER_CPU)  /**< Flag of inter cpu communications error */

  #define HEX_VAL_MAX                         (16)  /* Maximum value of a hex digit */
  
  #define COMMS_DEFAULT_BAUD                  (921600)
  #define BYTES_PER_FLOAT                     (4) /* Number of bytes in a float */
  #define BYTES_PER_UINT16                    (2) /* Number of bytes in a uint16_t */
  #define BYTES_PER_INT16                     (2) /* Number of bytes in a int16_t */
  #define BYTES_PER_UINT32                    (4) /* Number of bytes in a uint32_t */
  #define COMMS_I2C_MASK_READ                 (0x01)
  /***************************************
  * Structs
  ***************************************/ 

  /* Serial  */
  typedef struct {
    uint32_t (*write)(uint8_t val);                         /**< Write out a byte of data via UART */
    uint32_t (*print)(const char* str);                  /**< Write out a null-pointer string */
    uint32_t (*writeArray)(uint8_t *array, uint16_t len);  /**< Write multiple registers */
    uint32_t (*read)(uint8_t *result);                      /**< Read data from RX buffer */
    uint32_t (*readArray)(uint8_t *resultArray, uint16_t len);   /**< Read multiple values from a device */
    uint32_t (*getRxBufferSize)(uint8_t *result);           /**< Retrieve number of bytes pending in RX buffer*/
    uint32_t (*getTxBufferSize)(uint8_t *result);           /**< Retrieve number of bytes pending in TX buffer*/
    uint32_t error;
  } COMMS_UART_S;


  /* I2C */
  typedef struct {
    uint32_t (*write)(uint8_t deviceAddr, uint8_t regAddr, uint8_t val);                        /**< Write to an I2C register */
    uint32_t (*writeCmd)(uint8_t deviceAddr, uint8_t cmd);                                      /**< Send a command via I2C */
    uint32_t (*writeArray)(uint8_t deviceAddr, uint8_t regAddr, uint8_t *array, uint16_t len);  /**< Write multiple registers */
    uint32_t (*read)(uint8_t deviceAddr, uint8_t regAddr, uint8_t *result);  
    uint32_t (*readCmd)(uint8_t deviceAddr, uint8_t * result, uint16_t len);
    uint32_t (*readArray)(uint8_t deviceAddr, uint8_t regAddr, uint8_t *resultArray, uint16_t len);   /**< Read multiple values from a device */
  } COMMS_I2C_S;

  /* SPI */
  typedef struct {
    uint32_t (*setActive) (uint8_t id);
    uint32_t (*write) (uint8_t addr, uint8_t val);
    uint32_t (*read) (uint8_t addr, uint8_t *ret);
    uint32_t (*writeArray)(uint8_t addr, uint8_t* array, uint16_t len);
    uint32_t (*readArray) (uint8_t addr, uint8_t* array, uint16_t len);
    uint32_t (*getRxBufferSize)(uint8_t *result);           /**< Retrieve number of bytes pending in RX buffer*/
    uint32_t (*getTxBufferSize)(uint8_t *result);           /**< Retrieve number of bytes pending in TX buffer*/
    uint32_t (*clearRxBuffer)(void);
    uint32_t (*clearTxBuffer)(void);
  } COMMS_SPI_S;

  /* Float to Byte array */
  typedef union {
    float num;
    uint8_t bytes[BYTES_PER_FLOAT];
  } floatByte_U; 

    /* Float to Byte array */
  typedef union {
    uint32_t num;
    uint8_t bytes[BYTES_PER_UINT32];
  } uint32Byte_U; 

  /* uint16_t to Byte array */
  typedef union {
    uint16_t num;
    uint8_t bytes[BYTES_PER_UINT16];
  } uint16Byte_U; 

  /* int16_t to Byte array */
  typedef union {
    int16_t num;
    uint8_t bytes[BYTES_PER_UINT16];
  } int16Byte_U; 

  /* uint16_t to Float */
  typedef union {
    uint16_t num;
    float numF;
  } uint16Float_U; 

  /* int16_t to Float */
  typedef union {
    int16_t num;
    float numF;
  } int16Float_U; 

  /***************************************
  * Function Prototypes
  ***************************************/ 
  uint32_t Comms_validateUart(COMMS_UART_S *uart);
  uint32_t Comms_validateI2C(COMMS_I2C_S *i2c);
  uint32_t Comms_validateSpi(COMMS_SPI_S *spi);

  void float2Byte(float fIn, uint8_t* array);
  void byte2Float(uint8_t* array, float* fOut);
  void uint32ToByte(uint32_t in, uint8_t* array);
  void ByteToUint32(uint8_t* array, uint32_t* out);
  void uint16ToByte(uint16_t in, uint8_t* array);
  void ByteToUint16(uint8_t* array, uint16_t* out);
  void ByteToInt16(uint8_t* array, int16_t* out);
  void int16ToByte(int16_t in, uint8_t* array);
  void int16ToFloat(int16_t in, float* out);

#endif /* COMMS_H */

/* [] END OF FILE */

