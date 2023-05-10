/***************************************************************************
*                           Majestic Labs  © 2023
* File: template_main_psoc4.c
* Workspace: MJL Driver Templates
* Version: v1.0
* Author: Craig Cheney
*
* PCB: 
*
* Brief:
*   Template for the PSoC Main 
*
* Change Log:
*   2023.04.27 - Document created
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
//  #define MJL_DEBUG_LED            /* Test the battery charger LED */
//  #define MJL_DEBUG_UART            /* Test the UART Connection */
//  #define MJL_DEBUG_TIMER         /* Test the Timer */
  #define MJL_DEBUG_DUAL_TIMER         /* Test two timers feeding into each other */
  
#endif
/* -------------- END DEBUG CASE --------------  */
   


/* ############################# BEGIN PROGRAM ############################## */
uint32_t initHardware(void);
void ISR_timer(void);
void ISR_timerDual(void);

/* Global Variables */
MLJ_UART_S usb;
volatile bool flag_timer = false;
volatile bool flag_timer2 = false;


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
    uint8_t i = 0;
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
        else if (' ' == readVal) {
          uart_println(&usb, "dsaf");
          
          uart_printlnf(&usb, "%d", i++);
        }
      }
    }
  #elif defined MJL_DEBUG_TIMER
    /* Test the UART Connection */
    uart_printHeader(&usb, "MJL_DEBUG_TIMER", __DATE__, __TIME__);
    CyDelay(10);
    uart_println(&usb, "");
    for(;;) {
      /* Handle the UART */
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
      /* Handle the timer */
      if(flag_timer){
        flag_timer = false;
        uart_println(&usb, "Timer");
        pin_LED_G_Write(!pin_LED_G_Read());
      }
    }
  #elif defined MJL_DEBUG_DUAL_TIMER         
    /* Test two timers feeding into each other */
    uart_printHeader(&usb, "MJL_DEBUG_DUAL_TIMER", __DATE__, __TIME__);
    CyDelay(10);
    uart_println(&usb, "");
    for(;;) {
      /* Handle the UART */
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
        else if(' ' == readVal) {
          uint16_t timer1 = TimerFast1_ReadCounter();
          uint16_t timer2 = TimerFast2_ReadCounter();
          float sec = (float) timer2 * 0.001;
          #define CLK_48MHZ_TO_S 0.000000020833
          sec += (float) timer1 * CLK_48MHZ_TO_S;
          
//          uart_printlnf(&usb,"timer2:%i timer1:%i", timer2, timer1);
          uart_printlnf(&usb, "%.7f", sec);
        }
      }
      /* Handle the timer */
      if(flag_timer2){
        flag_timer2 = false;
        pin_LED_G_Write(!pin_LED_G_Read());
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
  /* Enable global interrupts. */
  CyGlobalIntEnable; 
  /* Start the UART */
  MJL_UART_CFG_S uartCfg = uart_cfg_default;
  uartCfg.hal_req_writeArray = uart_psoc4SCB_writeArrayBlocking;
  uartCfg.hal_req_read = uart_psoc4SCB_read;
  uartCfg.hal_opt_externalStart = uart_psoc4SCB_start;
  uartCfg.hal_opt_externalStop = uart_psoc4SCB_stop;
  error |= uart_init(&usb, &uartCfg);
  error |= uart_start(&usb);
  /* Start the timer */
  Timer_Start();
  TimerFast1_Start();
  TimerFast2_Start();
  interrupt_timer_StartEx(ISR_timer);
  interrupt_timerFast_StartEx(ISR_timerDual);
  
  
  
  return error;
}

/*******************************************************************************
* ISR Name: ISR_timer()
********************************************************************************
* Summary:
*   Triggered when the timer elapses. Updates the period if pending
* Interrupt: 
*   interrupt_btn
*
*******************************************************************************/
void ISR_timer(void){
  Timer_ClearInterrupt(Timer_INTR_MASK_TC);
  flag_timer = true;
}

/*******************************************************************************
* ISR Name: ISR_timerDual()
********************************************************************************
* Summary:
*   Triggered when the timer elapses. Updates the period if pending
* Interrupt: 
*   interrupt_btn
*
*******************************************************************************/
void ISR_timerDual(void){
  TimerFast2_ClearInterrupt(Timer_INTR_MASK_TC);
  flag_timer2 = true;
}



/* [] END OF FILE */
