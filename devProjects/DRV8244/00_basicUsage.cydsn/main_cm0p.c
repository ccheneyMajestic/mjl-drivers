/***************************************************************************
*                           Majestic Labs  © 2023
* File: main_cm0p.c
* Workspace: DRV8244
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
#include "hal_psoc6.h"
#include "DRV8244.h"

/* ####################### BEGIN PROGRAM CONFIGURATION ###################### */

#define MJL_DEBUG
/* ---------------- DEBUG CASE ----------------
* Uncomment ONE of the following
* Debugging will only occur when MJL_DEBUG is defined
*/
#ifdef MJL_DEBUG
//  #define MJL_DEBUG_LED            /* Test the battery charger LED */
//  #define MJL_DEBUG_UART            /* Test the UART Connection */
  #define MJL_DEBUG_DRV /* Basic usage the DRV driver chip */
#endif
/* -------------- END DEBUG CASE --------------  */
   
#define LED_ON (0)
#define LED_OFF (1)

/* ############################# BEGIN PROGRAM ############################## */
uint32_t initHardware(void);
void hal_drv_pin_sleep_write(uint32_t val);
void hal_drv_pin_drvoff_write(uint32_t val);
uint32_t hal_drv_pin_nfault_read(void);

/* Global Variables */
MLJ_UART_S usb;
drv8244_state_s drv;

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
      CyDelay(500);
      Cy_GPIO_Clr(pin_LED_G_PORT, pin_LED_G_NUM, LED_OFF);
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
  
  /* Test writing */
//  for(;;){
//    uart_println(&usb, "a");
//    CyDelay(1000);
//    Cy_GPIO_Inv(pin_LED_R_PORT, pin_LED_R_NUM);
//  }
  
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
        /* Reset both cores */
        NVIC_SystemReset();
      }
    }
  }
  #elif defined MJL_DEBUG_DRV 
    /* Basic usage the DRV driver chip */
    uart_printHeader(&usb, "MJL_DEBUG_DRV", __DATE__, __TIME__);
    CyDelay(10);
    uart_println(&usb, "");
    uart_println(&usb,"* Press 'Enter' to reset");
    uart_println(&usb,"* Press 'space' to toggle ENABLE");
    uart_println(&usb,"* Press '0' to toggle START/STOP");
    uart_println(&usb, "");
    
    /* Start the DRV8244 */
    drv8244_cfg_s drvCfg = drv8244_config_default;
    drvCfg.fn_delay_us = Cy_SysLib_DelayUs;
    drvCfg.fn_criticalSection_enter = Cy_SysLib_EnterCriticalSection;
    drvCfg.fn_criticalSection_exit = Cy_SysLib_ExitCriticalSection;    
    drvCfg.fn_pin_sleep_write = hal_drv_pin_sleep_write;
    drvCfg.fn_pin_drvoff_write = hal_drv_pin_drvoff_write;
    drvCfg.fn_pin_fault_read = hal_drv_pin_nfault_read;
    uint32_t error_drv = drv8244_init(&drv, &drvCfg);
    
    uart_printError(&usb, "drv8244_init", error_drv);
    

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
        /* Reset both cores */
        NVIC_SystemReset();
      }
      /* Enable the DRV */
      else if (' ' == readVal){
        if(drv.isEnabled){
          uart_println(&usb, "Disabling the DRV");
          drv8244_disable(&drv);
        }
        else {
          uart_println(&usb, "Enabling the DRV");
          drv8244_enable(&drv);
        }
      }
      /* START/STOP the DRV */
      else if ('0' == readVal) {
        if(!drv._running){
          uart_println(&usb, "Starting the DRV");
          drv8244_start(&drv);
        }
        else {
          uart_println(&usb, "Stopping the DRV");
          drv8244_stop(&drv);
        }
      }
      
//      bool in1 = Cy_GPIO_ReadOut(pin_DRV_DEBUG_PWM_PORT, pin_DRV_DEBUG_PWM_0_NUM);
      bool in1 = false;
      bool dir = Cy_GPIO_ReadOut(pin_DRV_DIR_PORT, pin_DRV_DIR_0_NUM);
      bool drvoff = Cy_GPIO_ReadOut(pin_DRV_DRVOFF_PORT, pin_DRV_DRVOFF_0_NUM);
      bool nSleep = Cy_GPIO_ReadOut(pin_DRV_nSLEEP_PORT, pin_DRV_nSLEEP_0_NUM);
      bool nFault = Cy_GPIO_Read(pin_DRV_nFAULT_PORT, pin_DRV_nFAULT_0_NUM);
      uart_printlnf(&usb, "IN1: %b, IN2: %b, AWAKE: %b, ACTIVE: %b, FAULT: %b", \
          in1, dir, nSleep, !drvoff, !nFault);
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
    

    return error;
}

/*******************************************************************************
* Function Name: hal_drv_pin_sleep_write()
********************************************************************************
* \brief
*   Control the nSLEEP pin
*
* \return
*  Error code of the operation
*******************************************************************************/
void hal_drv_pin_sleep_write(uint32_t val) {
  Cy_GPIO_Write(pin_DRV_nSLEEP_0_PORT, pin_DRV_nSLEEP_0_NUM, val);
}

/*******************************************************************************
* Function Name: hal_drv_pin_drvoff_write()
********************************************************************************
* \brief
*   Control the DRVOFF pin
*
* \return
*  Error code of the operation
*******************************************************************************/
void hal_drv_pin_drvoff_write(uint32_t val) {
  Cy_GPIO_Write(pin_DRV_DRVOFF_0_PORT, pin_DRV_DRVOFF_0_NUM, val);
}

/*******************************************************************************
* Function Name: hal_drv_pin_nfault_read()
********************************************************************************
* \brief
*   Read the nFault pin
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t hal_drv_pin_nfault_read(void){
  return Cy_GPIO_Read(pin_DRV_nFAULT_PORT, pin_DRV_nFAULT_NUM); 
}

/* [] END OF FILE */
