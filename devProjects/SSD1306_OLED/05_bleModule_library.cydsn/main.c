/***************************************************************************
*                           Majestic Labs  © 2023
* File: main.c
* Workspace: strenthLoop_v4
* Project Name: pcbaDev
* Version: v1.0
* Author: Craig Cheney
*
* PCB: SLv4.0.0
*
* Brief:
*   This project is for application development on the v4.0.0 custom StrengthLoop PCBA
*
* Change Log:
*   2023.04.02 - Document created
********************************************************************************/
#include "project.h"
#include "OLED_SSD1306.h"
#include "LTC6915.h"
#include "mjl_font.h"
#include "mjl_errors.h"
#include "mjl_uart.h"
#include "SL_hal_psoc4_CYBLE22014.h"

/* ####################### BEGIN PROGRAM CONFIGURATION ###################### */

#define MJL_DEBUG
/* ---------------- DEBUG CASE ----------------
* Uncomment ONE of the following
* Debugging will only occur when MJL_DEBUG is defined
*/
#ifdef MJL_DEBUG
//    #define MJL_DEBUG_LED            /* Test the battery charger LED */
//    #define MJL_DEBUG_UART            /* Test the UART Connection */
//    #define MJL_DEBUG_AUX             /* Test the Auxiliary load switch & battery monitor */
//    #define MJL_DEBUG_BTN             /* Test the buttons */
//    #define MJL_DEBUG_BATT_MON        /* Test the battery monitor */
    #define MJL_DEBUG_DISPLAY_ON        /* Toggle display */
//    #define MJL_DEBUG_DISPLAY         /* Turn on the display */
//    #define MJL_DEBUG_INA_GAIN        /* Adjust the gain on the instrumentation amp */
//    #define MJL_DEBUG_INA             /* View the output of the INA */
//    #define MJL_DEBUG_INA_REF         /* Adjust the reference to the INA */
//    #define MJL_DEBUG_FLASH           /* Make contact with the flash */
#endif
/* -------------- END DEBUG CASE --------------  */
   


/* ############################# BEGIN PROGRAM ############################## */
uint32_t initHardware(void);

/* Global Variables */
MLJ_UART_S usb;
//COMMS_SPI_S spi;
ssd1306_state_s display;

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
    #elif defined MJL_DEBUG_AUX             
    /* Test the Auxiliary load switch */
        for(;;){
            pin_EN_AUX_Write(AUX_OFF);
            CyDelay(1000);
            pin_EN_AUX_Write(AUX_ON);
            CyDelay(1000);
        }
    #elif defined MJL_DEBUG_BTN             
    /* Test the buttons */
        printHeader(&usb, "MJL_DEBUG_BTN");
        
        bool btn1 = pin_BTN1_Read();
        bool btn1_prev = btn1;
        bool btn2 = pin_BTN2_Read();
        bool btn2_prev = btn2;
        
        for(;;){
            /* Read the buttons */
            btn1 = pin_BTN1_Read();
            btn2 = pin_BTN2_Read();
            /* Print out the new state */
            if(btn1 != btn1_prev) {
                btn1_prev = btn1;
                if(btn1) {
                    printLn(&usb, "Btn1 pressed");
                } else {
                    printLn(&usb, "Btn1 released");
                }
            }
            if(btn2 != btn2_prev) {
                btn2_prev = btn2;
                if(btn2) {
                    printLn(&usb, "Btn2 pressed");
                } else {
                    printLn(&usb, "Btn2 released");
                }
            }
            /* Handle UART input */
            uint8_t readVal = 0;
            usb.read(&readVal);
            if(0 != readVal) {                
                /* Reset on Enter */
                if('\r' == readVal) {
                    print(&usb, "\r\nResetting...");
                    CyDelay(1000);
                    CySoftwareReset();  
                }
            }
        }
        
    #elif defined MJL_DEBUG_BATT_MON        
    /* Test the battery monitor */
        uart_printHeader(&usb, "MJL_DEBUG_BATT_MON", __DATE__, __TIME__);
        
        /* Enable the battery monitor*/
        pin_EN_AUX_Write(AUX_ON);
        for(;;) {
            ADC_StartConvert();
            ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
            uint16_t count = ADC_GetResult16(ADC_CHAN_BATT_MON);
            float mon_voltage = ADC_CountsTo_Volts(ADC_CHAN_BATT_MON, count);
            float batt_voltage = BATT_MON_SCALE * mon_voltage;
            uart_printlnf(&usb, "Battery count: %i, %.4f V, %.3f V", count, mon_voltage, batt_voltage);
            CyDelay(1000);
        }
        /* Enable the AUX Switch*/

    #elif defined MJL_DEBUG_FLASH           
    /* Make contact with the flash */
        
    #elif defined MJL_DEBUG_DISPLAY_ON        
      /* Toggle display */
      uart_printHeader(&usb, "MJL_DEBUG_DISPLAY_ON", __DATE__, __TIME__);
      
      CyDelay(1000);
      uart_println(&usb, "");
      
//        uart_printErrorStatus(&usb, "Start display", error, 0);
      SSD1306_clearScreen(&display);
      bool isDisplayDataShown = true;
      for(;;) {
        /* Handle UART */
        uint8_t readVal = 0;
        uart_read(&usb, &readVal);
        if(0 != readVal) {
          /* Reset on Enter */
          if('\r' == readVal) {
            uart_print(&usb, "\r\nResetting...");
            CyDelay(1000);
            CySoftwareReset();  
          }
        }
        /* Toggle between all on and all off */
        CyDelay(1000);
        isDisplayDataShown = !isDisplayDataShown;
        uint8_t cmdWord = 0;
        if(isDisplayDataShown){
          uart_println(&usb, "Display data shown");
          cmdWord = SSD1306_CMD_ALL_RAM;
        }
        else {
          uart_println(&usb, "Display all on");
          cmdWord = SSD1306_CMD_ALL_ON;
        }
        SSD1306_writeCommandArray(&display, &cmdWord, 1);
      }
    /* End MJL_DEBUG_SCREEN_START */ 
    #elif defined MJL_DEBUG_DISPLAY         
    /* Turn on the display */
        printHeader(&usb, "MJL_DEBUG_DISPLAY");
        CyDelay(10);
        print(&usb, "\r\n");

        /* Enable the battery monitor*/
        pin_EN_AUX_Write(AUX_ON);
        CyDelay(1);
        
        printLn(&usb,"* Press 'Enter' to reset");
        printLn(&usb,"* Press ' ' to draw and increment a number");
        printLn(&usb,"* Press '0' to toggle between display all on and data");
        printLn(&usb,"* Press '1' to write a page of data to on");
        printLn(&usb,"* Press '2' to clear the data");
        
        display_position_s upperLeft;
        upperLeft.origin_col = 0;
        upperLeft.origin_row = 0;
        upperLeft.size_cols = 8;
        upperLeft.size_rows = 16;
        upperLeft.repeat_num = 1;
        upperLeft.repeat_spacing = 0;
        
        uint8_t digit =0;

        bool isDisplayDataShown = true;
        for(;;) {
            uint8_t readVal = 0;
            usb.read(&readVal);
            if(0 != readVal) {                
                /* Reset on Enter */
                if('\r' == readVal) {
                    print(&usb, "\r\nResetting...");
                    CyDelay(1000);
                    CySoftwareReset();  
                }
                else if (' ' == readVal) {
                    printLn(&usb, "%i", digit);
                    uint32_t error = SSD1306_setDigits(&display, &digit, &upperLeft);
                    if(++digit > 9){
                        digit =0;
                    }
                }
                else if('0' == readVal) {
                    isDisplayDataShown = !isDisplayDataShown;
                    uint8_t cmdWord = 0;
                    if(isDisplayDataShown){
                        printLn(&usb, "Display data shown");
                        cmdWord = SSD1306_CMD_ALL_RAM;
                    }
                    else {
                        printLn(&usb, "Display all on");
                        cmdWord = SSD1306_CMD_ALL_ON;
                    }
                    SSD1306_writeCommandArray(&display, &cmdWord, 1);
                }
                else if('1' == readVal) {
                    printLn(&usb, "Writing display row");
                    uint8_t dataArray[LEN_ROW];
                    memset(dataArray, 0xFF, LEN_ROW);
                    SSD1306_writeDataArray(&display, dataArray, LEN_ROW);
                }
                else if('2' == readVal) {
                    printLn(&usb, "Clearing Display data");
                    SSD1306_clearScreen(&display);
                }
            }
        }
    #elif defined MJL_DEBUG_INA_GAIN        
    /* Adjust the gain on the instrumentation amp */
        /* Test the UART Connection */
        printHeader(&usb, "MJL_DEBUG_INA_GAIN");
        CyDelay(10);
        print(&usb, "\r\n");
        printLn(&usb,"* Press 'Enter' to reset");
        printLn(&usb,"* Press ' ' to increment the gain");
        uint32_t error = 0;
        uint8_t gain =0;
        
        /* Enable AUX */
        pin_EN_AUX_Write(AUX_ON);
        for(;;) {
            uint8_t readVal = 0;
            usb.read(&readVal);
            if(0 != readVal) {
                /* Echo UART */
                usb.write(readVal);   
                /* Reset on Enter */
                if('\r' == readVal) {
                    print(&usb, "\r\nResetting...");
                    CyDelay(1000);
                    CySoftwareReset();  
                }
                /* Incrment the gain */
                else if (' ' == readVal) {
                    error |= spi_scbWriteArrayBlocking(SL_SPI_ID_INA, &gain, 1); 
                    if(gain>9){gain=0;}
                    CyDelay(10);
                    printLn(&usb, "Gain:%i, error:%i", gain, error);
                    gain++;
                }
            }
        }
        
    #elif defined MJL_DEBUG_INA             
        /* View the output of the INA */
        printHeader(&usb, "MJL_DEBUG_INA");
        CyDelay(10);
        print(&usb, "\r\n");
        printLn(&usb,"* Press 'Enter' to reset");
        printLn(&usb,"* Press ' ' to increment the gain");
        printLn(&usb,"* Press '0' to toggle the ADC");
        
        uint32_t error = 0;
        uint8_t gainWord =0;
        bool isLoggingEnabled = false;
        
        /* Enable AUX */
        pin_EN_AUX_Write(AUX_ON);
        CyDelay(100);
        error |= spi_scbWriteArrayBlocking(SL_SPI_ID_INA, &gainWord, 1); 
        printLn(&usb, "Gain word: %i, Gain:%i, error:%i", gainWord, 0, error);
        gainWord++;
        for(;;) {
            uint8_t readVal = 0;
            usb.read(&readVal);
            if(0 != readVal) {
                /* Echo UART */
                usb.write(readVal);   
                /* Reset on Enter */
                if('\r' == readVal) {
                    print(&usb, "\r\nResetting...");
                    CyDelay(1000);
                    CySoftwareReset();  
                }
                /* Incrment the gain */
                else if (' ' == readVal) {
                    error |= spi_scbWriteArrayBlocking(SL_SPI_ID_INA, &gainWord, 1); 
                    CyDelay(10);
                    uint16_t gain = gainWord == 0 ? 0 : 1 << (gainWord -1);
                    printLn(&usb, "Gain word: %i, Gain:%i, error:%i", gainWord, gain, error);
                    gainWord++;
                    if(gainWord>13){gainWord=0;}
                }
                /* Toggle logging */
                else if ('0' == readVal) {
                    isLoggingEnabled = !isLoggingEnabled;
                }
            }
            if(isLoggingEnabled){
                ADC_StartConvert();
                ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
                uint16_t count = ADC_GetResult16(ADC_CHAN_INA);
                float strain = ADC_CountsTo_Volts(ADC_CHAN_INA, count);
                printLn(&usb, "INA count: %i, %.4f V", count, strain);
                CyDelay(1000);
            }
            
        }
    #elif defined MJL_DEBUG_INA_REF         
    /* Adjust the reference to the INA */
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
    /* Start the SPI component */
    SPI_Start();
    /* Start the ADC */
//    ADC_Start();
    /* start the OLED */
    ssd1306_cfg_s display_cfg;
    display_cfg.fn_spi_writeArrayBlocking = spi_scbWriteArrayBlocking;
    display_cfg.fn_pin_reset_write = pin_oled_reset_write_dummy;
    display_cfg.fn_pin_dataCommand_write = pin_DISP_DC_Write;
    display_cfg.fn_delayMs = CyDelay; /* Not an actual warning */
    display_cfg.fullWindow.colStart = 0;
    display_cfg.fullWindow.colEnd = 127;
    display_cfg.fullWindow.pageStart = 0;
    display_cfg.fullWindow.pageEnd = 7;
    display_cfg.spi_slaveId = SL_SPI_ID_DISPLAY;
    error |= SSD1306_init(&display, &display_cfg);
    error |= SSD1306_start(&display);
    
    return error;
}




/* [] END OF FILE */
