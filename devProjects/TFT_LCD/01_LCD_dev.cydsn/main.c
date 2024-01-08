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
#include "TFT_hal_psoc4.h"
#include "TFT_GC9107.h"
#include <stdlib.h>

/* ####################### BEGIN PROGRAM CONFIGURATION ###################### */

#define MJL_DEBUG
/* ---------------- DEBUG CASE ----------------
* Uncomment ONE of the following
* Debugging will only occur when MJL_DEBUG is defined
*/
#ifdef MJL_DEBUG
//  #define MJL_DEBUG_LED            /* Test the RGB LED */
//  #define MJL_DEBUG_UART            /* Test the UART Connection */
//  #define MJL_DEBUG_RESET     /* Test the reset/3V3 connection */
//  #define MJL_DEBUG_PWM       /* Test the PWM for the LCD */
//  #define MJL_DEBUG_BOOT    /* Boot the LCD */
//  #define MJL_DEBUG_ROW       /*Write a row of data */
  #define MJL_DEBUG_RECEIVE_IMAGE /* Receive an image over uart */
  
#endif
/* -------------- END DEBUG CASE --------------  */

/* ############################# BEGIN PROGRAM ############################## */
uint32_t initHardware(void);
uint32_t asciiPixelToRgb(const char *csvRow, rgb_s * pixel);
uint32_t asciiImageToRgb(char *csv);
void printPixel(MLJ_UART_S *uart, rgb_s *pixel);
/* Global Variables */


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
      #define BLINK_DELAY (500)
      
      pin_LED_R_Write(LED_OFF);
      pin_LED_G_Write(LED_OFF);
      pin_LED_B_Write(LED_OFF);
      for(;;) {
        pin_LED_G_Write(LED_ON);
        CyDelay(BLINK_DELAY);
        pin_LED_G_Write(LED_OFF);
        CyDelay(BLINK_DELAY);
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
    #elif defined MJL_DEBUG_RESET     
      /* Test the reset/3V3 connection */
      uart_printHeader(&usb, "MJL_DEBUG_RESET", __DATE__, __TIME__);
      uart_println(&usb, "Press 'enter' to reset");
      uart_println(&usb, "");
      
      bool isAuxActive = false;
      for(;;) {
        /* Toggle the reset line */
        pin_LCD_3V3_Write(isAuxActive);
        pin_LED_R_Write(isAuxActive);
        pin_LED_G_Write(isAuxActive);
        pin_LED_B_Write(isAuxActive);
        uart_printlnf(&usb, "3V3 enabled: %b", isAuxActive);
        isAuxActive = !isAuxActive;
        CyDelay(500);
        /* Handle the UART */
        uint8_t readVal = 0;
        uint32_t readError = uart_read(&usb, &readVal);
        if(!readError) {          
          /* Reset on Enter */
          if('\r' == readVal) {
            uart_print(&usb, "\r\nResetting...");
            CyDelay(1000);
            CySoftwareReset();  
          }
        }
      }
    # elif defined MJL_DEBUG_PWM
      /* Test the PWM for the LCD */
            uart_printHeader(&usb, "MJL_DEBUG_PWM", __DATE__, __TIME__);
      uart_println(&usb, "Press 'enter' to reset");
      uart_println(&usb, "Press '0' to 0% Duty cycle");
      uart_println(&usb, "Press '1' to 25% Duty cycle");
      uart_println(&usb, "Press '2' to 50% Duty cycle");
      uart_println(&usb, "Press '3' to 75% Duty cycle");
      uart_println(&usb, "Press '4' to 100% Duty cycle");
      
      
      
      uart_println(&usb, "");
      
      lcd_init();
      lcd_wakeup();

      for(;;) {

        /* Handle the UART */
        uint8_t readVal = 0;
        uint32_t readError = uart_read(&usb, &readVal);
        if(!readError) {          
          /* Reset on Enter */
          if('\r' == readVal) {
            uart_print(&usb, "\r\nResetting...");
            CyDelay(1000);
            CySoftwareReset();  
          }
          else if('0' == readVal){
            PWM_LCD_WriteCompare(0);
          }
          else if('1' == readVal){
            PWM_LCD_WriteCompare(25);
          }
          else if('2' == readVal){
            PWM_LCD_WriteCompare(50);
          }
          else if('3' == readVal){
            PWM_LCD_WriteCompare(75);
          }
          else if('4' == readVal){
            PWM_LCD_WriteCompare(99);
          }
        }
      }
    #elif defined MJL_DEBUG_BOOT    
      /* Boot the LCD */
      /* Test the PWM for the LCD */
      uart_printHeader(&usb, "MJL_DEBUG_LCD_BOOT", __DATE__, __TIME__);
      uart_println(&usb, "Press 'enter' to reset uC");
      uart_println(&usb, "Press 'space' to cycle between colors");
      uart_println(&usb, "Press '1' to toggle display");
      uart_println(&usb, "Press '2' Invert colors");
      
      uart_println(&usb, "");
      
      lcd_wakeup();
      
      bool isLcdOn = true;
      bool isDisplayInverted = false;
      RGB_COLORS_T colorName = BLACK;
      
      for(;;) {
        /* Handle the UART */
        uint8_t readVal = 0;
        uint32_t readError = uart_read(&usb, &readVal);
        if(!readError) {          
          /* Reset on Enter */
          if('\r' == readVal) {
            uart_print(&usb, "\r\nResetting...");
            CyDelay(1000);
            CySoftwareReset();  
          }
          else if (' ' == readVal){
            if (++colorName>0b111){colorName=BLACK;}
            colorName2Rgb(colorName, &color, 0xFF);
            lcd_drawColor(&window, &color);
          }
          else if ('1' == readVal){
            isLcdOn = !isLcdOn;
            lcd_setDisplayMode(isLcdOn);
            uart_printlnf(&usb, "isLcdOn: %b", isLcdOn);
          }
          else if ('2' == readVal){
            uart_println(&usb, "Invert display"); 
            lcd_writeCommand(isDisplayInverted ? LCD_CMD_INVERT_OFF :  LCD_CMD_INVERT_ON);
            isDisplayInverted = !isDisplayInverted;
          }
        }
      }
    #elif defined MJL_DEBUG_ROW       
      /*Write a row of data */
      uart_printHeader(&usb, "MJL_DEBUG_ROW", __DATE__, __TIME__);
      uart_println(&usb, "Press 'enter' to reset uC");
      uart_println(&usb, "Press 'space' to write rows");
      
      uart_println(&usb, "");
      
      lcd_wakeup();
      
      uint8_t rowIndex = 55;
      for(;;) {
        /* Handle the UART */
        uint8_t readVal = 0;
        uint32_t readError = uart_read(&usb, &readVal);
        if(!readError) {          
          /* Reset on Enter */
          if('\r' == readVal) {
            uart_print(&usb, "\r\nResetting...");
            CyDelay(1000);
            CySoftwareReset();  
          }
          else if (' ' == readVal){
            LCD_WINDOW_S rowWindow;
            rowWindow.start.x=0;
            rowWindow.start.y = rowIndex;
            rowWindow.end.x = LCD_PIXEL_HORZ -1 ;
            rowWindow.end.y = rowIndex;
            rowWindow.numCols = LCD_PIXEL_HORZ;
            rowWindow.numRows = 1;
            
            rgb_s color;
            colorName2Rgb(BLUE, &color, 255);
            lcd_drawColor(&rowWindow, &color);
            if(++rowIndex == LCD_PIXEL_VERT){rowIndex=0;}
            uart_printlnf(&usb, "Write row: %d", rowIndex);
          }
        }
      }
    #elif defined MJL_DEBUG_RECEIVE_IMAGE
      /* Receive an image over uart */
      uart_printHeader(&usb, "MJL_DEBUG_RECEIVE_IMAGE", __DATE__, __TIME__);
      uart_println(&usb, "Press 'enter' to reset uC");
      uart_println(&usb, "Press 'space' reset the screen");
      uart_println(&usb, "Press '1' to send response");
      
      
      uart_println(&usb, "");      
      lcd_wakeup();

      for(;;) {
        /* Handle the UART */
        uint8_t readVal = 0;
        uint32_t readError = uart_read(&usb, &readVal);
        if(!readError) {          
          /* Reset on Enter */
          if('\r' == readVal) {
            uart_print(&usb, "\r\nResetting...");
            CyDelay(1000);
            CySoftwareReset();  
          }
          else if (' ' == readVal){
            colorName2Rgb(WHITE, &color, 0xFF);
            lcd_drawColor(&window, &color);
          }
          else if ('1' == readVal){
            char testBuffer[10] = "32,32,31\r\n";
            rgb_s testPixel;
            asciiPixelToRgb((char *) testBuffer, &testPixel); 
            printPixel(&usb, &testPixel);
            
          }
          #define START_OF_TRANSMISSION (0x02)
          #define END_OF_TRANSMISSION (0x04)
          else if (START_OF_TRANSMISSION == readVal){
            pin_LED_R_Write(LED_ON);
            pin_LED_G_Write(LED_OFF);
            pin_LED_B_Write(LED_OFF);
            # define STR_BUFFER_LEN 15
            char strBuffer[STR_BUFFER_LEN]= {0x00};
            rgb_s pixelRow[LCD_PIXEL_HORZ];
            
            uint8_t colIndex = 0;
            uint8_t rowIndex = 0;
            uint8_t i = 0;
            
            LCD_WINDOW_S rowWindow;
            rowWindow.start.x=0;
            rowWindow.end.x = 127;
            rowWindow.numCols = 128;
            rowWindow.numRows = 1;
            
            /* Receive bytes */
            for(;;){
              readError = uart_read(&usb, &readVal);
              if(!readError){
                /* Stop on end of transmission */
                if(END_OF_TRANSMISSION == readVal){
                  pin_LED_R_Write(LED_OFF);
                  pin_LED_G_Write(LED_ON);
                  pin_LED_B_Write(LED_OFF);
                  break;
                }
                /* end of line */
                else if('\n' == readVal){
                  rgb_s pixel;
                  asciiPixelToRgb((char *) strBuffer, &pixel); 
                  pixelRow[colIndex++] = pixel;
                  i=0;
                  memset(strBuffer, 0x00, STR_BUFFER_LEN);
                  /* Write the row out */
                  if(colIndex == LCD_PIXEL_HORZ) {
                    rowWindow.start.y = rowIndex;
                    rowWindow.end.y = rowIndex;
//                    printPixel(&usb, &pixelRow[0]);
                    lcd_writeRgb16_window(&rowWindow, pixelRow);
                    rowIndex++;
                    colIndex=0;
                  }
                }
                /* Data */
                else {
                  strBuffer[i++] = readVal; 
                  /* Overflow */
                  if(i == STR_BUFFER_LEN){break;}
                }
              }
            }
            printPixel(&usb, &pixelRow[colIndex-1]);
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
  /* Start the RGB LED */
  pin_LED_R_Write(LED_ON);
  pin_LED_G_Write(LED_ON);
  pin_LED_B_Write(LED_ON);
  /* Start the UART */
  MJL_UART_CFG_S uartCfg = uart_cfg_default;
  uartCfg.hal_req_writeArray = uart_psoc4SCB_writeArrayBlocking;
  uartCfg.hal_req_read = uart_psoc4SCB_read;
  uartCfg.hal_opt_externalStart = uart_psoc4SCB_start;
  uartCfg.hal_opt_externalStop = uart_psoc4SCB_stop;
  error |= uart_init(&usb, &uartCfg);
  error |= uart_start(&usb);
  /* Start the SPI */ 
  SPI_Start();
  /* Start the Hardware for the LCD */
  lcd_init();
  
  return error;
}

/*******************************************************************************
* Function Name: asciiPixelToRgb()
********************************************************************************
* \brief
*   Convert an ASCII Pixel to the RGB struct
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t asciiPixelToRgb(const char *csvRow, rgb_s * pixel) {
  uint32_t error = 0;
  char * savePointer;
  /* Get Character token */
  char *rToken = strtok_r((char *) csvRow, ",", &savePointer);
  char *gToken = strtok_r(NULL, ",", &savePointer);
  char *bToken = strtok_r(NULL, "\r", &savePointer);
  /* Ensure all are present */
  if(rToken == NULL || gToken == NULL || bToken == NULL) {error|=ERROR_INVALID;}
  if(!error){
    /* Save as Pixel */
    pixel->r = atoi(rToken);
    pixel->g = atoi(gToken);
    pixel->b = atoi(bToken);
  }
  return error;
  
}

/*******************************************************************************
* Function Name: asciiImageToRgb()
********************************************************************************
* \brief
*   Convert an ASCII Image to a row of images 
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t asciiImageToRgb(char *csv){
  uint32_t error = 0;
  char *save;
  uint16_t i = 0;
  rgb_s imageRow[LCD_PIXEL_HORZ];
  /* Parse the CSV into rows */
  for(char *row = strtok_r(csv, "\r\n", &save); row!= NULL; row = strtok_r(NULL, "\r\n", &save)){
    rgb_s pixel;
    asciiPixelToRgb(row, &pixel);
    imageRow[i++]=pixel;
    if(i==LCD_PIXEL_HORZ){
      // Draw the row 
    }
    printPixel(&usb, &pixel);
  }
  
  
  return error;
}



/*******************************************************************************
* Function Name: printPixel()
********************************************************************************
* \brief
*   Print the value of a pixel
*
* \return
*  Error code of the operation
*******************************************************************************/
void printPixel(MLJ_UART_S *uart, rgb_s *pixel){
  uart_printlnf(uart, "R:%d, G:%d, B:%d", pixel->r, pixel->g, pixel->b);
}

/* [] END OF FILE */
