/***************************************************************************
*                           Majestic Labs  © 2023
* File: template_main_psoc6.c
* Workspace: MJL Driver Templates
* Version: v1.0
* Author: Craig Cheney
*
* PCB: 
*
* Brief:
*   Template for the PSoC6 Main for the CM0+ Core
*
* Change Log:
*   2023.12.31 - Document created
********************************************************************************/
#include "project.h" /* Cypress project */
#include "mjl_errors.h"
#include "mjl_uart.h"
#include "mjl_spi.h"
#include "hal_psoc6.h"

/* ####################### BEGIN PROGRAM CONFIGURATION ###################### */

#define MJL_DEBUG
/* ---------------- DEBUG CASE ----------------
* Uncomment ONE of the following
* Debugging will only occur when MJL_DEBUG is defined
*/
#ifdef MJL_DEBUG
    #define MJL_DEBUG_LED            /* Test the battery charger LED */
//    #define MJL_DEBUG_UART            /* Test the UART Connection */
#endif
/* -------------- END DEBUG CASE --------------  */


/* ############################# BEGIN PROGRAM ############################## */
uint32_t initHardware(void);

/* Global Variables */
MLJ_UART_S usb;
MJL_SPI_S spi;

/* Main Program */
#ifndef MJL_DEBUG
/*******************************************************************************
* Function Name: main()
********************************************************************************
* Summary:
*   The top-level application function for the project.
*
*******************************************************************************/
int main(void) {
  initHardware();
  __enable_irq(); /* Enable global interrupts. */
  /* Enable CM4.  CY_CORTEX_M4_APPL_ADDR must be updated if CM4 memory layout is changed. */
  Cy_SysEnableCM4(CY_CORTEX_M4_APPL_ADDR); 

  for(;;){
    
  }
}
#endif /* ifndef MJL_DEBUG*/
/* End Main Program */


/* ############################ BEGIN DEBUGGER ############################## */
#ifdef MJL_DEBUG
/*******************************************************************************
* DEBUGGER: main()
********************************************************************************
* Summary:
*   Debugging function for the PCB hardware
*******************************************************************************/
int main(void){
  #warning "MJL_DEBUG is enabled"
  initHardware();
  
  /* Test Cases */
  #ifdef MJL_DEBUG_LED
    /* blink the LED */
    for(;;) {
      Cy_GPIO_Write(pin_LED_G_PORT, pin_LED_G_NUM, LED_ON);
      Cy_GPIO_Write(pin_LED_R_PORT, pin_LED_R_NUM, LED_OFF);
      CyDelay(500);
      Cy_GPIO_Write(pin_LED_G_PORT, pin_LED_G_NUM, LED_OFF);
      Cy_GPIO_Write(pin_LED_R_PORT, pin_LED_R_NUM, LED_ON);
      CyDelay(500);
    }
  /* End MJL_DEBUG_LED */
  #elif defined MJL_DEBUG_UART             
  /* Test the UART Connection */
  uart_printHeader(&usb, "MJL_DEBUG_UART", __DATE__, __TIME__);
  CyDelay(10);
  uart_println(&usb, "");
  uart_println(&usb,"* Press 'Enter' to reset");
  uart_println(&usb, "");
  Cy_GPIO_Write(pin_LED_G_PORT, pin_LED_G_NUM, LED_ON);
  
  for(;;) {
    uint8_t readVal = 0;
    uint32_t readError = uart_read(&usb, &readVal);
    if(!readError) {
      /* Echo UART */
      uart_write(&usb, readVal);     
      /* Toggle the Red LED */
      Cy_GPIO_Inv(pin_LED_R_PORT, pin_LED_R_NUM);
      /* Reset on Enter */
      if('\r' == readVal) {
        uart_print(&usb, "\r\nResetting...");
        CyDelay(1000);
        /* Reset both cores */
        NVIC_SystemReset();
      }
    }
  }
  #endif 
  /* Fall through Infinite loop */
  for(;;){}
}
#endif /* MJL_DEBUG */


/* ####################### BEGIN FUNCTION DEFINITIONS ####################### */

/*******************************************************************************
* Function Name: initHardware()
********************************************************************************
* \brief
*   Configures the hardware to start
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t initHardware(void) {
  uint32_t error = 0;
  /* Start the UART */
  MJL_UART_CFG_S uartCfg = uart_cfg_default;
  uartCfg.hal_req_writeArray = uart_psoc6SCB_writeArrayBlocking;
  uartCfg.hal_req_read = uart_psoc6SCB_read;
  uartCfg.hal_opt_externalStart = uart_psoc6SCB_start;
  uartCfg.hal_opt_externalStop = uart_psoc6SCB_stop;
  error |= uart_init(&usb, &uartCfg);
  error |= uart_start(&usb);
  /* Start the SPI Component */
  MJL_SPI_CFG_S spiCfg = spi_cfg_default;
  spiCfg.opt_hal_externalStart = spi_psoc6SCB_start;
  spiCfg.opt_hal_externalStop = spi_psoc6SCB_stop;
  spiCfg.req_hal_writeArray_blocking = spi_psoc6SCB_writeArray_blocking;
  spiCfg.req_hal_read = spi_psoc6SCB_read;
  spiCfg.req_hal_setActive = spi_psoc6SCB_setActive;
  spiCfg.req_hal_getRxBufferNum = spi_psoc6SCB_getRxBufferNum;
  spiCfg.req_hal_getTxBufferNum = spi_psoc6SCB_getTxBufferNum;
  spiCfg.req_hal_clearRxBuffer = spi_psoc6SCB_clearRxBuffer;
  spiCfg.req_hal_clearTxBuffer = spi_psoc6SCB_clearTxBuffer;
  error |= spi_init(&spi, &spiCfg);
  error |= spi_start(&spi);

  return error;
}

/* [] END OF FILE */
