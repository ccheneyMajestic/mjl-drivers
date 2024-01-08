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
  #include <mjl_errors.h>
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
  typedef struct MLJ_SPI_S MLJ_SPI_T;
  /* Configuration Structure */
  typedef struct {
    uint32_t (*req_hal_writeArray)(const uint8_t *array, uint16_t len);  /* Write data into the TX buffer */
    uint32_t (*req_hal_read)(uint8_t *result);                           /* Move data from the RX buffer to the result */
    uint32_t (*opt_hal_externalStart)(MLJ_SPI_T *const);                 /* Optional External start function */
    uint32_t (*opt_hal_externalStop)(MLJ_SPI_T *const);                  /* Optional External stop function */
  } MJL_SPI_CFG_S;

  /* Serial State Object   */
  typedef struct {
    uint32_t (*req_hal_writeArray)(const uint8_t *array, uint16_t len);  /* Write data into the TX buffer */
    uint32_t (*req_hal_read)(uint8_t *result);         /* Move data from the RX buffer to the result */
    uint32_t (*opt_hal_externalStart)(MLJ_SPI_T *const);                 /* Optional External start function */
    uint32_t (*opt_hal_externalStop)(MLJ_SPI_T *const);                  /* Optional External stop function */

    bool _init;
    bool _running;
    bool isLoggingEnabled;
  } MLJ_SPI_S;

  /* Default config struct */
  extern const MJL_SPI_CFG_S spi_cfg_default;
  /***************************************
  * Function declarations 
  ***************************************/
  /* State Operations */
  uint32_t spi_init(MLJ_SPI_S *const state, MJL_SPI_CFG_S *const cfg);
  uint32_t spi_start(MLJ_SPI_S *const state);
  uint32_t spi_stop(MLJ_SPI_S *const state);

  // uint32_t spi_write(MLJ_SPI_S *const state, uint8_t data);
  // uint32_t spi_read(MLJ_SPI_S *const state, uint8_t * data);
  // uint32_t spi_writeArray(MLJ_SPI_S *const state, uint8_t * array, uint16_t len);
  // uint32_t spi_readArray(MLJ_SPI_S *const state, uint8_t * array, uint16_t len);
  // uint32_t spi_write_reverse(MLJ_SPI_S *const state, uint8_t * array, uint16_t len);
  // uint32_t spi_print(MLJ_SPI_S *const state, const char * pszFmt);
  // uint32_t spi_println(MLJ_SPI_S *const state, const char * pszFmt);
  // uint32_t spi_printf(MLJ_SPI_S* state, const char *pszFmt,...);
  // uint32_t spi_printlnf(MLJ_SPI_S* state, const char *pszFmt,...);
    
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