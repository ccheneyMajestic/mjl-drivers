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
* 2023.04.26  - Document Created
********************************************************************************/
/* Header Guard */
#ifndef MJL_SPI_H
  #define MJL_SPI_H
  /***************************************
  * Included files
  ***************************************/
  #include <stdbool.h>
  #include <stdint.h>
  /***************************************
  * Macro Definitions
  ***************************************/

  /***************************************
  * Enumerated types
  ***************************************/

  /***************************************
  * Structures 
  ***************************************/
   /* Forward declare struct */
  typedef struct MJL_SPI_S MJL_SPI_T;
  /* Configuration Structure */
  typedef struct {
    uint32_t (*req_hal_writeArray)(const uint8_t *array, uint16_t len);  /* Write data into the TX buffer */
    uint32_t (*req_hal_read)(uint8_t *result);                           /* Move data from the RX buffer to the result */
    uint32_t (*req_hal_setActive) (uint8_t id);                          /* Set the Slave with given ID active */
    uint32_t (*req_hal_getRxBufferNum) (uint8_t *result);                /* Get the number of elements in the Receive Buffer */
    uint32_t (*req_hal_getTxBufferNum) (uint8_t *result);                /* Get the number of elements in the Transmit Buffer */
    uint32_t (*req_hal_clearRxBuffer) (void);                            /* Clear the Receive Buffer */
    uint32_t (*req_hal_clearTxBuffer) (void);                            /* Clear the Transmit Buffer */
    uint32_t (*opt_hal_externalStart)(MJL_SPI_T *const);                 /* Optional External start function */
    uint32_t (*opt_hal_externalStop)(MJL_SPI_T *const);                  /* Optional External stop function */
  } MJL_SPI_CFG_S;

  /* Serial State Object   */
  typedef struct {
    uint32_t (*req_hal_writeArray)(const uint8_t *array, uint16_t len);  /* Write data into the TX buffer */
    uint32_t (*req_hal_read)(uint8_t *result);                           /* Move data from the RX buffer to the result */
    uint32_t (*req_hal_setActive) (uint8_t id);                          /* Set the Slave with given ID active */
    uint32_t (*req_hal_getRxBufferNum) (uint8_t *result);                /* Get the number of elements in the Receive Buffer */
    uint32_t (*req_hal_getTxBufferNum) (uint8_t *result);                /* Get the number of elements in the Transmit Buffer */
    uint32_t (*req_hal_clearRxBuffer) (void);                            /* Clear the Receive Buffer */
    uint32_t (*req_hal_clearTxBuffer) (void);                            /* Clear the Transmit Buffer */
    uint32_t (*opt_hal_externalStart)(MJL_SPI_T *const);                 /* Optional External start function */
    uint32_t (*opt_hal_externalStop)(MJL_SPI_T *const);                  /* Optional External stop function */

    bool _init;
    bool _running;
    bool isLoggingEnabled;
  } MJL_SPI_S;

  /* Default config struct */
  extern const MJL_SPI_CFG_S spi_cfg_default;
  /***************************************
  * Function declarations 
  ***************************************/
  /* State Operations */
  uint32_t spi_init(MJL_SPI_S *const state, MJL_SPI_CFG_S *const cfg);
  uint32_t spi_start(MJL_SPI_S *const state);
  uint32_t spi_stop(MJL_SPI_S *const state);

  uint32_t spi_write(MJL_SPI_S *const state, uint8_t id, uint8_t data);
  uint32_t spi_read(MJL_SPI_S *const state,  uint8_t id, uint8_t * data);
  uint32_t spi_writeArray(MJL_SPI_S *const state, uint8_t id, uint8_t * array, uint16_t len);
  uint32_t spi_readArray(MJL_SPI_S *const state,  uint8_t id, uint8_t * array, uint16_t len);
  uint32_t spi_getRxBufferNum(MJL_SPI_S *const state, uint8_t * result);
  uint32_t spi_getTxBufferNum(MJL_SPI_S *const state, uint8_t * result);
  uint32_t spi_clearRxBuffer(MJL_SPI_S *const state, uint8_t * result);
  uint32_t spi_clearTxBuffer(MJL_SPI_S *const state, uint8_t * result);
  // uint32_t spi_setActive(MJL_SPI_S *const state, uint8_t id);

    
#endif /* MJL_SPI_H */
/* [] END OF FILE */


// /***************************************************************************
// *                              Impact Biosystems © 2021
// *                               
// *
// * File: PSoC6_SPI.h
// * Workspace: 05_Gen2
// * Project: scannerDriver_v4.0
// * Version: 1.0.0
// * Authors: E. Burba
// * 
// * PCB: Scanner Driver v4.0.0
// * mcuType: PSoC 6 BLE Module
// *
// * Brief:
// *   Header for PSoC6_SPI.c
// *
// * 2021.08.03 - EB Document Created
// ********************************************************************************/

// /* Header Guard */
// #ifndef SPI_PSOC6_H
//     #define SPI_PSOC6_H
//     /***************************************
//     * Included files
//     ***************************************/
//     #include <stdint.h>
//     #include "micaCommon.h"
//     #include "micaComms.h"
//     /***************************************
//     * Macro Definitions
//     ***************************************/
    
//     //    #define SPI_PSOC_ERROR_NONE         (0) /**< No error occurred (or no error detection present) */
// //    
// //    #define SPI_PSOC_FLAG_NO_DATA       (0) /**< No Data was received */
// //    #define SPI_PSOC_FLAG_PENDING_RX    (1) /**< Data was already in the queue */
// //    
// //    #define SPI_PSOC_ERROR_NO_DATA      (1u << SPI_PSOC_FLAG_NO_DATA) /**< No Data was received */
// //    #define SPI_PSOC_ERROR_PENDING_RX   (1u << SPI_PSOC_FLAG_PENDING_RX) /**< Data was already in the queue */
    
    
//     /***************************************
//     * Enumerated Types
//     ***************************************/

    
//     /***************************************
//     * Structures
//     ***************************************/

//     /***************************************
//     * Function declarations 
//     ***************************************/ 
//     uint32_t spiPsoc6_start(COMMS_SPI_S *spi);
//     uint32_t spiPsoc6_setActive(uint8_t id);
//     uint32_t spiPsoc6_write(uint8_t addr, uint8_t val);
//     uint32_t spiPsoc6_read(uint8_t addr, uint8_t *ret);
//     uint32_t spiPsoc6_writeArray(uint8_t addr, uint8_t* array, uint16_t len);
//     uint32_t spiPsoc6_readArray(uint8_t addr, uint8_t* array, uint16_t len);
//     uint32_t spiPsoc6_getRxBufferSize(uint8_t *result);
//     uint32_t spiPsoc6_getTxBufferSize(uint8_t *result);
//     uint32_t spiPsoc6_clearRxBuffer(void);
//     uint32_t spiPsoc6_clearTxBuffer(void);
// #endif