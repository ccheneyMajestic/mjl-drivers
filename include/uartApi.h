/***************************************************************************
*                                 MICA  © 2019
*
* File: uartApi.h
* Workspace: micaOS
* Version: v2.0.0
* Author: Craig Cheney
*
* Brief:
*   
*
* 2019.10.01  - Document Created
********************************************************************************/

/* Header Guard */
#ifndef UART_API_H
  #define UART_API_H
  /***************************************
  * Included files
  ***************************************/
  #include <stdint.h>
  #include "comms.h"
  #include "options.h"
  /***************************************
  * Macro Definitions
  ***************************************/
  #define CLEAR_SCREEN_CMD    "\033[2J\033[1;1H"
  #define CLEAR_LINE_CMD      "\033[K\r"
  #define CMD_LINE_UP         "\033[1A"
  #define CMD_CLEAR_LINEUP    "\r\033[1A\033[K"
  /***************************************
  * Enumerated Types
  ***************************************/

  
  /***************************************
  * Structures
  ***************************************/
  
  /***************************************
  * Function declarations 
  ***************************************/
  uint32_t print(COMMS_UART_S* uart, const char *pszFmt,...);
  uint32_t printHeaderDT(COMMS_UART_S* uart, const char* name, const char *date, const char* time);
  uint32_t printHeaderDTB(COMMS_UART_S* uart, const char* name, const char *date, const char* time);
  uint32_t printErrorStatus(COMMS_UART_S* uart, const char* name, uint32_t error, uint32_t subError);
  uint32_t uart_hex2Ascii(uint8_t hex, uint8_t* ascii);
  uint32_t uart_compareReg(COMMS_UART_S* uart, const char* name, uint16_t actual, uint16_t expected);
  uint32_t writeArray_rev(COMMS_UART_S* uart, uint8_t *array, uint16_t len);
  uint32_t parseAsciiFloat(uint8_t* buf, uint16_t len, float* result);
  uint32_t getInputFloat(COMMS_UART_S* uart, float* result);


#ifdef USE_DEBUG_UART
  #define printLn(uart, ...) do {print(uart, __VA_ARGS__); print(uart, "\r\n"); } while(0) 
  #define printHeader(uart, name) printHeaderDT(uart, name, __DATE__, __TIME__)
  #define printHeaderBuild(uart, name) printHeaderDTB(uart, name, __DATE__, __TIME__)
#else
  #define printLn(uart, ...) 
  #define printHeader(uart, name) 
#endif
#endif /* UART_API_H */
/* [] END OF FILE */
