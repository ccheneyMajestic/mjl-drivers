/***************************************************************************
*                           Majestic Labs  © 2023
* File: main.c
* Workspace: Creator Template
* Version: v1.0
* Author: Craig Cheney
*
* PCB: 
*
* Brief:
*   Template for the PSoC 4200 Main 
*
* Change Log:
*   2024.05.05 - Document created
********************************************************************************/
#include "project.h" /* Cypress project */
#include "mjl_errors.h"
#include "mjl_uart.h"
#include "hal_psoc4.h"

/* ####################### BEGIN PROGRAM CONFIGURATION ###################### */

#define MJL_DEBUG
/* ---------------- DEBUG CASE ----------------
* Uncomment ONE of the following
* Debugging will only occur when MJL_DEBUG is defined
*/
#ifdef MJL_DEBUG
//    #define MJL_DEBUG_LED            /* Test the battery charger LED */
    #define MJL_DEBUG_UART            /* Test the UART Connection */
#endif
/* -------------- END DEBUG CASE --------------  */
   


/* ############################# BEGIN PROGRAM ############################## */
uint32_t initHardware(void);

/* Global Variables */
MLJ_UART_S usb;

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
        /* Set pin_CHRG_STAT to Open Drain & Input */
        /* NOTE: USB must be plugged in, with no battery attached for this to work */
        /* blink the LED */
        for(;;) {
            pin_CHRG_STAT_Write(0);
            CyDelay(500);
            pin_CHRG_STAT_Write(1);
            CyDelay(500);
        }
        
    /* End MJL_DEBUG_LED */
    #elif defined MJL_DEBUG_UART           
        /* Test the UART Connection */
        uart_printHeader(&usb, "MJL_DEBUG_UART", __DATE__, __TIME__);
        CyDelay(10);
        uart_println(&usb, "");
        for(;;) {
            uint8_t readVal = 0;
            uint32_t readError = uart_read(&usb, &readVal);
            if(!readError) {
                /* Echo UART */
                uart_write(&usb, readVal);                
                /* Reset on Enter */
                if('\r' == readVal) {
                    uart_print(&usb, "\r\nResetting...");
                    CyDelay(1000);
                    CySoftwareReset();  
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
    uartCfg.hal_req_writeArray = uart_psoc4SCB_writeArrayBlocking;
    uartCfg.hal_req_read = uart_psoc4SCB_read;
    uartCfg.hal_opt_externalStart = uart_psoc4SCB_start;
    uartCfg.hal_opt_externalStop = uart_psoc4SCB_stop;
    error |= uart_init(&usb, &uartCfg);
    error |= uart_start(&usb);
    return error;
}

/* [] END OF FILE */