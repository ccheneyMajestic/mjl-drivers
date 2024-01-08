/***************************************************************************
*                                Majestic Labs © 2023
* File: template_hal_psoc4.h
* Workspace: MJL Driver Templates
* Version: v1.0.0
* Author: C. Cheney
* Target: PSoC4 CYBLE_222014-01 Module
*
* Brief: Contains wrappers for the MJL library from the Cypress PSoC4 libraries
*
* 2023.09.11  - Document Created
********************************************************************************/
/* Header Guard */
#ifndef TFT_HAL_PSOC4_H
  #define TFT_HAL_PSOC4_H
  /***************************************
  * Included files
  ***************************************/
  #include <stdint.h>
  #include "mjl_uart.h"
  /***************************************
  * Macro Definitions
  ***************************************/

  #define LED_ON  (0)
  #define LED_OFF (1)
  #define LEN_ROW 128
  
  #define AUX_OFF             (0) /* Disable 3V3_AUX */
  #define AUX_ON              (1) /* Enable 3V3_AUX */  
  #define SPI_ID_LCD        (0) /* SPI Slave ID of the Display */
  
  #define SPI_CS_ACTIVE     (0) /* Active Chip Select */
  #define SPI_CS_INACTIVE   (1) /* Inactive chip select */
  /***************************************
  * Enumerated types
  ***************************************/

  /***************************************
  * Structures 
  ***************************************/
  extern MLJ_UART_S usb;
  /***************************************
  * Function declarations 
  ***************************************/
  uint32_t uart_psoc4SCB_start(MLJ_UART_T *const state);
  uint32_t uart_psoc4SCB_stop(MLJ_UART_T *const state);
  uint32_t uart_psoc4SCB_writeArrayBlocking(const uint8_t *array, uint16_t len);
  uint32_t uart_psoc4SCB_read(uint8_t *data);
  uint32_t hal_spi_scbWriteArrayBlocking(uint8_t slaveId, const uint8_t * cmdArray, uint16_t len);
  uint32_t hal_spi_scbReadArrayBlocking(uint8_t slaveId, uint8_t * buffer, uint16_t len);

    
#endif /* TFT_HAL_PSOC4_H */
/* [] END OF FILE */
