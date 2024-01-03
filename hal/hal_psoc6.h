/***************************************************************************
*                                Majestic Labs © 2023
* File: hal_psoc6.h
* Workspace: MJL Hardware Abstraction Layer (HAL) Library 
* Version: v1.0.0
* Author: C. Cheney
* Target: PSoC6
*
* Brief: Contains wrappers for the MJL library from the Cypress PSoC4 libraries
*
* 2023.12.31  - Document Created
********************************************************************************/
/* Header Guard */
#ifndef HAL_PSOC6_H
  #define HAL_PSOC6_H
  /***************************************
  * Included files
  ***************************************/
  #include <stdint.h>
  #include "mjl_uart.h"
  /***************************************
  * Macro Definitions
  ***************************************/
  #define LED_ON (0)
  #define LED_OFF (1)
  /***************************************
  * Enumerated types
  ***************************************/

  /***************************************
  * Structures 
  ***************************************/

  /***************************************
  * Function declarations 
  ***************************************/
  uint32_t uart_psoc6SCB_start(MLJ_UART_T *const state);
  uint32_t uart_psoc6SCB_stop(MLJ_UART_T *const state);
  uint32_t uart_psoc6SCB_writeArrayBlocking(const uint8_t *array, uint16_t len);
  uint32_t uart_psoc6SCB_read(uint8_t *data);
  uint32_t spi_scbWriteArrayBlocking(uint8_t slaveId, uint8_t * cmdArray, uint16_t len);
  uint32_t spi_scbReadArrayBlocking(uint8_t slaveId, uint8_t * buffer, uint16_t len);

    
#endif /* HAL_PSOC6_H */
/* [] END OF FILE */
