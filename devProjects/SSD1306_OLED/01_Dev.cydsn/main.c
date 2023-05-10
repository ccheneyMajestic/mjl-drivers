/***************************************************************************
*                           Majestic Labs  © 2023
* File: main.c
* Workspace: SSD1306_OLED
* Project Name: 01_Dev
* Version: v1.0
* Author: Craig Cheney
*
* PCB: PSoC4 BLE Pioneer Dev Kit
*
* Brief:
*   This project is for driver development for the SSD1306 OLED Display. The
*   Display is 128x64 and is configured to use SPI for communication. 
*   https://www.adafruit.com/product/326
*
* Change Log:
*   2023.02.18 - Document created
********************************************************************************/
#include "project.h"
#include "RGB.h"
#include "uartApi.h"
#include "PSoC_serial.h"
#include "PSoC_SPI.h"


/* ####################### BEGIN PROGRAM CONFIGURATION ###################### */

#define MJL_DEBUG
/* ---------------- DEBUG CASE ----------------
* Uncomment ONE of the following
* Debugging will only occur when MJL_DEBUG is defined
*/
#ifdef MJL_DEBUG
//    #define MJL_DEBUG_LEDS           /* Test the onboard RGB LED */
//    #define MJL_DEBUG_UART           /* Test the UART Connection */
  #define MJL_DEBUG_SCREEN_CLEAR      /* Turn the screen on and clear it */
//    #define MJL_DEBUG_SCREEN_ON           /* Turn the entire screen on */
//    #define MJL_DEBUG_NUMBERS           /* Draw the numbers */
//    #define MJL_DEBUG_DRAW_UI          /* Draw the UI */
    
    
    
#endif
/* -------------- END DEBUG CASE --------------  */
   


/* ############################# BEGIN PROGRAM ############################## */
#define DELAY 500

#define SPI_ID_OLED             (0) /* Slave ID of the OLED */

#define SSD1306_RESET_DISASSERT (1) /* Deassert the reset pin */
#define SSD1306_RESET_ASSERT    (0) /* Assert the reset pin */
#define SSD1306_PAGE_HEIGHT     (8) /* Pixels per page */
#define SSD1306_NUM_PAGE        (8)  /* Number of pages in the display */
#define SSD1306_NUM_COLS        (128)  /* Number of cols in the display */
#define SSD1306_NUM_ROWS        (64)  /* Number of Rows in the display */



#define SSD1306_DC_DATA         (1) /* Write data */
#define SSD1306_DC_COMMAND      (0) /* Write command */

#define SSD1306_CMD_ALL_ON      (0xA5) /* Command to turn all LEDs on */
#define SSD1306_CMD_ALL_RAM     (0xA4) /* Command to turn all LEDs to match what's in RAM */
#define SSD1306_CMD_CHARGE_PUMP             (0x8D) /* Command to address the charge pump */
#define SSD1306_CMD_CHARGE_PUMP_ON          (0x14) /* Enable Charge pump */
#define SSD1306_CMD_CHARGE_PUMP_OFF         (0x10) /* Disable Charge pump */
#define SSD1306_CMD_ON                      (0xAF) /* Enable the Display (wakeup) */
#define SSD1306_CMD_SLEEP                   (0xAE) /* Disable the Display (sleep) */

#define SSD1306_CMD_ADDRESS_MODE            (0x20) /* Set the addressing mode */
#define SSD1306_CMD_ADDRESS_MODE_HORIZONTAL (0x00) /* Horizontal Addressing mode */
#define SSD1306_CMD_ADDRESS_MODE_VERTICAL   (0x01) /* Vertical Addressing mode */
#define SSD1306_CMD_ADDRESS_MODE_PAGE       (0x02) /* Page Addressing mode (default) */

#define SSD1306_CMD_COLUMN_ADDR             (0x21) /* Set the column address */
#define SSD1306_CMD_PAGE_ADDR               (0x22) /* Set the page address */

#define UI_ICON_BATTERY_LEN             (16) /* number of bytes in the battery icon */
#define UI_ICON_HISTORY_LEN             (48) /* number of bytes in the history icon */
#define UI_ICON_RESET_LEN               (48) /* number of bytes in the rest icon */
#define UI_TEXT_8x16_LEN                     (16) /* Number of byte in an 8x16 character */

#define DISPLAY_LEN_16x32  (64)





/* Bitmaps for digits 0-9 */
const uint8_t zero_8x16[UI_TEXT_8x16_LEN]     = {0xFE, 0xFE, 0x03, 0x03, 0x03, 0x03, 0xFE, 0xFE, 0x7F, 0x7F, 0xC0, 0xC0, 0xC0, 0xC0, 0x7F, 0x7F};
const uint8_t one_8x16[UI_TEXT_8x16_LEN]      = {0x00, 0x10, 0x1C, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xFF, 0xFF, 0xC0, 0xC0, 0xC0};
const uint8_t two_8x16[UI_TEXT_8x16_LEN]      = {0x1C, 0x9E, 0x83, 0x83, 0x83, 0xFF, 0xFE, 0x00, 0xFF, 0xFF, 0xC1, 0xC1, 0xC1, 0xC1, 0xC0, 0x00};
const uint8_t three_8x16[UI_TEXT_8x16_LEN]    = {0x1E, 0x1F, 0x03, 0xC3, 0xC3, 0xFF, 0x7E, 0x00, 0x78, 0xF8, 0xC0, 0xC1, 0xC1, 0xFF, 0x7F, 0x00};
const uint8_t four_8x16[UI_TEXT_8x16_LEN]     = {0x80, 0xE0, 0xF8, 0x9C, 0x8E, 0xFF, 0xFF, 0x80, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFF, 0xFF, 0x01};
const uint8_t five_8x16[UI_TEXT_8x16_LEN]     = {0x7F, 0xFF, 0xC3, 0xC3, 0xC3, 0xC3, 0x83, 0x00, 0x78, 0xF8, 0xC0, 0xC0, 0xC0, 0xFF, 0x7F, 0x00};
const uint8_t six_8x16[UI_TEXT_8x16_LEN]      = {0xFC, 0xFE, 0x87, 0x83, 0x83, 0x9F, 0x9E, 0x00, 0x3F, 0xFF, 0xC1, 0xC1, 0xC1, 0xFF, 0x7F, 0x00};
const uint8_t seven_8x16[UI_TEXT_8x16_LEN]    = {0x03, 0x03, 0x03, 0x83, 0xE3, 0xFF, 0x3F, 0x07, 0x00, 0xC0, 0xF8, 0xFF, 0x0F, 0x01, 0x00, 0x00};
const uint8_t eight_8x16[UI_TEXT_8x16_LEN]    = {0x3E, 0xFE, 0xC3, 0xC3, 0xC3, 0xC3, 0xFE, 0x3E, 0x3F, 0x7F, 0xC0, 0xC0, 0xC0, 0xC0, 0x7F, 0x3F};
const uint8_t nine_8x16[UI_TEXT_8x16_LEN]     = {0x7C, 0xFE, 0x83, 0x83, 0x83, 0x83, 0xFE, 0xFC, 0x38, 0x78, 0xE1, 0xC1, 0xC1, 0xC1, 0xFF, 0x7F};
const uint8_t* digits_8x16[10]  = {zero_8x16, one_8x16, two_8x16, three_8x16, four_8x16, five_8x16, six_8x16, seven_8x16, eight_8x16, nine_8x16};
/* Icon bitmaps */
const uint8_t batteryOutline[UI_ICON_BATTERY_LEN]= {0x3C, 0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF};
const uint8_t battery_1bar[UI_ICON_BATTERY_LEN]=   {0x3C, 0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xBD, 0xBD, 0x81, 0xFF};

const uint8_t historyOutline[UI_ICON_HISTORY_LEN]= {0x00, 0xF8, 0x08, 0x08, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x08, 0x08, 0xF8, 0x00,
                                   0x00, 0xFF, 0x00, 0x00, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x00, 0x00, 0xFF, 0x00, 
                                   0xC0, 0xDF, 0xD0, 0xD0, 0xD1, 0xD1, 0xD1, 0xD1, 0xD1, 0xD1, 0xD1, 0xD1, 0xD0, 0xD0, 0xDF, 0xC0};
const uint8_t resetOutline[UI_ICON_RESET_LEN]  = {0x00, 0x00, 0x80, 0xE0, 0x30, 0x18, 0x18, 0x18, 0x18, 0x98, 0x18, 0x18, 0x30, 0x60, 0xC0, 0x00, 
                                   0x04, 0x0C, 0x1F, 0x0C, 0x04, 0x00, 0x20, 0x30, 0x18, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x81, 0xFF, 
                                   0xC0, 0xC0, 0xC0, 0xC6, 0xCC, 0xD8, 0xD8, 0xD8, 0xD8, 0xD8, 0xD8, 0xD8, 0xCC, 0xC6, 0xC3, 0xC0};
/* Text bitmaps */
const uint8_t M_8x16[UI_TEXT_8x16_LEN]        = {0xFF, 0xF8, 0xE0, 0x00, 0x00, 0xE0, 0xF8, 0xFF, 0xFF, 0x00, 0x01, 0x0F, 0x0F, 0x01, 0x00, 0xFF};
const uint8_t A_8x16[UI_TEXT_8x16_LEN]        = {0x00, 0xE0, 0x7C, 0x1F, 0x1F, 0x7C, 0xE0, 0x00, 0xFC, 0x1F, 0x18, 0x18, 0x18, 0x18, 0x1F, 0xFC};
const uint8_t X_8x16[UI_TEXT_8x16_LEN]        = {0x01, 0x07, 0x3C, 0xF0, 0xF0, 0x3C, 0x07, 0x01, 0x80, 0xE0, 0x3C, 0x0F, 0x0F, 0x3C, 0xE0, 0x80};
const uint8_t* maxText_bitmaps[3]       = {M_8x16, A_8x16, X_8x16};
const uint8_t K_8x16[UI_TEXT_8x16_LEN]        = {0xFF, 0xFF, 0xC0, 0xC0, 0xF8, 0xFE, 0x07, 0x01, 0xFF, 0xFF, 0x03, 0x01, 0x01, 0x3F, 0xFE, 0xF0};
const uint8_t G_8x16[UI_TEXT_8x16_LEN]        = {0xFC, 0xFF, 0x01, 0x01, 0x01, 0x83, 0x8F, 0x8E, 0x3F, 0x7F, 0xC0, 0x80, 0x80, 0x81, 0xFF, 0x7F};
const uint8_t* kgText_bitmaps[2] = {K_8x16, G_8x16};


/* Controll the display window */
typedef struct {
    uint8_t pageStart;
    uint8_t pageEnd;
    uint8_t colStart;
    uint8_t colEnd;  
} display_window_s;

typedef struct {
    uint8_t origin_col;
    uint8_t origin_row;
    uint8_t size_cols;
    uint8_t size_rows;
    uint8_t repeat_num;
    uint8_t repeat_spacing;
} display_position_s;


/* Function declarations */ 
uint32_t initHardware(void); 
uint32_t spi_writeArrayBlocking(uint8_t slaveId, const uint8_t * cmdArray, uint16_t len);
uint32_t tokenizeNumber(uint16_t val, uint8_t *outArray);


uint32_t SSD1306_start(void);
uint32_t SSD1306_writeCommandArray(uint8_t * cmdArray, uint8_t len);
uint32_t SSD1306_writeDataArray(const uint8_t * dataArray, uint16_t len);
uint32_t SSD1306_setWindow(display_window_s *const window);
uint32_t SSD1306_windowFromPos(display_position_s *const pos, uint8_t instanceNum, display_window_s *const window);
uint32_t SSD1306_drawDigit_8x16(uint8_t num);
uint32_t SSD1306_clearScreen();
uint32_t SSD1306_setDigits(uint8_t* digits, display_position_s *const pos);
uint32_t SSD1306_setLetters(const uint8_t **letters, display_position_s *const pos);
uint32_t SSD1306_create16x32(const uint8_t * inObject, uint8_t * outObject);
uint32_t SSD1306_setDigits_16x32(uint8_t *digits, display_position_s *const pos);



/* Global Variables */
RGB_S rgb;
COMMS_UART_S usb;
COMMS_SPI_S spi;

display_position_s batteryIcon;
display_position_s maxForceDigits;
display_position_s forceDigits;
display_position_s exerciseDigit;
display_position_s maxLetters;
display_position_s historyIcon;
display_position_s resetIcon;
display_position_s unitLetters;

display_window_s fullWindow;



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
    #ifdef MJL_DEBUG_LEDS
        /* Test the onboard LEDs
        Expeted outcome: 
        0. All LEDs off
        1. Red LED on
        2. Green LED on
        3. Blue LED on
        4. Yellow (RB)
        5. Cyan (BG)
        6. Magenta (RG)
        7. White (RGB) LEDs on
        */
        /* Infinite loop */
        for(;;){
            /* Off */
            RGB_Write(&rgb, RGB_None);
            CyDelay(DELAY);
            /* Red */
            RGB_Write(&rgb, RGB_Red);
            CyDelay(DELAY);
            /* Green */
            RGB_Write(&rgb, RGB_Green);
            CyDelay(DELAY);
            /* Blue */
            RGB_Write(&rgb, RGB_Blue);;
            CyDelay(DELAY);
            /* Yellow */
            RGB_Write(&rgb, RGB_Yellow);
            CyDelay(DELAY);
            /* Cyan */
            RGB_Write(&rgb, RGB_Cyan);
            CyDelay(DELAY);
            /* Magenta */
            RGB_Write(&rgb, RGB_Magenta);
            CyDelay(DELAY);
            /* White */
            RGB_Write(&rgb, RGB_White);
            CyDelay(DELAY);
        }
        
    /* End MJL_DEBUG_LEDS */
    #elif defined MJL_DEBUG_UART           
        /* Test the UART Connection */
        printHeader(&usb, "MJL_DEBUG_UART");
        CyDelay(10);
        print(&usb, "\r\n");
        RGB_Write(&rgb, RGB_Red);

        for(;;) {
            uint8_t readVal = 0;
            usb.read(&readVal);
            if(0 != readVal) {
                /* Echo UART */
                usb.write(readVal);   
                /* Toggle the Red LED */
                RGB_R_Toggle(&rgb);
                
                /* Reset on Enter */
                if('\r' == readVal) {
                    print(&usb, "\r\nResetting...");
                    CyDelay(1000);
                    CySoftwareReset();  
                }
            }
        }
    /* End MJL_DEBUG_UART */    
    #elif defined MJL_DEBUG_SCREEN_CLEAR      
      /* Turn the screen on and clear it */
      printHeader(&usb, "MJL_DEBUG_SCREEN_CLEAR");
      printLn(&usb, "* Press 'enter' to reset");

      
      CyDelay(10);
      print(&usb, "\r\n");
      RGB_Write(&rgb, RGB_Blue);

      SSD1306_clearScreen();
      uint8_t columnStart = 0;
      uint8_t columnEnd = 7;
      uint8_t pageStart = 0;
      uint8_t pageEnd = 1;
      uint8_t dataArray[6] = {
        SSD1306_CMD_COLUMN_ADDR, columnStart, columnEnd,
        SSD1306_CMD_PAGE_ADDR, pageStart, pageEnd
      };
      SSD1306_writeCommandArray(dataArray, 6);
      SSD1306_writeDataArray((uint8_t *)one_8x16, 16);
      
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
        }
      }
    #elif defined MJL_DEBUG_SCREEN_ON           
      /* Turn the entire screen on */
      printHeader(&usb, "MJL_DEBUG_SCREEN_ON");
      printLn(&usb, "* Press 'enter' to reset");
      printLn(&usb, "* Press ' ' to Start the display");
      printLn(&usb, "* Press '0' to issue ALL ON command");
      printLn(&usb, "* Press '1' to issue DATA command");
      printLn(&usb, "* Press '2' to write the data ON ");
      printLn(&usb, "* Press '3' to write the data OFF");
      printLn(&usb, "* Press '4' to Set the display window to 0,0");
      printLn(&usb, "* Press '5' to set the window");
      printLn(&usb, "* Press '6' to write out a '1'");
      printLn(&usb, "* Press '7' to Write out a '2'");
      
      CyDelay(10);
      print(&usb, "\r\n");
      RGB_Write(&rgb, RGB_Blue);

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
            printLn(&usb, "Writing to display");
            RGB_G_Toggle(&rgb);
            SSD1306_start();
          }

          else if('0' == readVal) {
            printLn(&usb, "Display all on");
            RGB_R_Toggle(&rgb);
            uint8_t cmdWord = SSD1306_CMD_ALL_ON;
            SSD1306_writeCommandArray(&cmdWord, 1);
          }
          else if('1' == readVal) {
            printLn(&usb, "Display data");
            RGB_R_Toggle(&rgb);
            uint8_t cmdWord = SSD1306_CMD_ALL_RAM;
            SSD1306_writeCommandArray(&cmdWord, 1);
          }
          else if('2' == readVal) {
            printLn(&usb, "Writing display data");
            RGB_G_Toggle(&rgb);
            uint8_t dataArray[128] = {
                0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
            };
            SSD1306_writeDataArray(dataArray, 1);
          }
          else if('3' == readVal) {
            printLn(&usb, "Clearing Display data");
            RGB_G_Toggle(&rgb);
            SSD1306_clearScreen();
//            uint8_t dataArray[128] = {
//              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//            };
//            uint8_t i;
//            for(i=0; i < 8; i++){
//              SSD1306_writeDataArray(dataArray, 128);
//            }
          }
          else if('4' == readVal) {
            printLn(&usb, "Set window to 0-7, 0-1");
            uint8_t columnStart = 0;
            uint8_t columnEnd = 7;
            uint8_t pageStart = 0;
            uint8_t pageEnd = 1;
            uint8_t dataArray[6] = {
              SSD1306_CMD_COLUMN_ADDR, columnStart, columnEnd,
              SSD1306_CMD_PAGE_ADDR, pageStart, pageEnd
            };
            SSD1306_writeCommandArray(dataArray, 6);
          }
          else if('5' == readVal) {
            printLn(&usb, "Set window to 15-30, 1-2");
            uint8_t columnStart = 15;
            uint8_t columnEnd = 30;
            uint8_t pageStart = 1;
            uint8_t pageEnd = 2;
            uint8_t dataArray[6] = {
                SSD1306_CMD_COLUMN_ADDR, columnStart, columnEnd,
                SSD1306_CMD_PAGE_ADDR, pageStart, pageEnd
            };
            SSD1306_writeCommandArray(dataArray, 6);
          }

          else if('6' == readVal) {
            SSD1306_writeDataArray((uint8_t *)one_8x16, 16);
          }
          else if('7' == readVal) {
            SSD1306_writeDataArray((uint8_t *)two_8x16, 16);
          }
        }
      }
    /* End MJL_DEBUG_SCREEN_ON */
    #elif defined MJL_DEBUG_NUMBERS           
        /* Draw the numbers */
        printHeader(&usb, "MJL_DEBUG_NUMBERS");
        printLn(&usb,"* Press ' ' to increment the numbers");
        printLn(&usb,"* Press '0' to clear the screen");
        
        CyDelay(10);
        print(&usb, "\r\n");
        RGB_Write(&rgb, RGB_Blue);
        uint16_t count = 0;
        
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
                else if ('0' == readVal) {
                    printLn(&usb, "Blank screen");
                    SSD1306_clearScreen();
                }
                else if (' ' == readVal) {
                    printLn(&usb, "%i", count);
                    uint8_t digitsBuffer[4];
                    tokenizeNumber(count++, digitsBuffer);
                    SSD1306_setDigits(digitsBuffer, &maxForceDigits);
                    if(count > 9999){
                        count =0;
                    }
                }
            }
        }
    /* End MJL_DEBUG_NUMBERS */
    #elif defined MJL_DEBUG_DRAW_UI          
        /* Draw the UI */
        printHeader(&usb, "MJL_DEBUG_DRAW_UI");
        printLn(&usb,"* Press 'Enter' to Restart");
        printLn(&usb,"* Press ' ' to increment the numbers");
        
        CyDelay(10);
        print(&usb, "\r\n");
        RGB_Write(&rgb, RGB_Blue);
        /* Buffers */
        display_window_s activeWindowBuffer;
        uint8_t digitsBuffer[4];
        uint16_t loopCount = 0;
        
        /**** Drawing ****/
        SSD1306_clearScreen();
        /* Battery Icon */
        SSD1306_windowFromPos(&batteryIcon,0,&activeWindowBuffer);
        SSD1306_setWindow(&activeWindowBuffer);
        SSD1306_writeDataArray(battery_1bar, UI_ICON_BATTERY_LEN);
        /* History Icon */
        SSD1306_windowFromPos(&historyIcon,0,&activeWindowBuffer);
        SSD1306_setWindow(&activeWindowBuffer);
        SSD1306_writeDataArray(historyOutline, UI_ICON_HISTORY_LEN);
        /* Reset Icon */
        SSD1306_windowFromPos(&resetIcon,0,&activeWindowBuffer);
        SSD1306_setWindow(&activeWindowBuffer);
        SSD1306_writeDataArray(resetOutline, UI_ICON_RESET_LEN);
        /* Max Force */
        tokenizeNumber(130, digitsBuffer);
        SSD1306_setDigits(digitsBuffer, &maxForceDigits);
        /* Max text */
        SSD1306_setLetters(maxText_bitmaps, &maxLetters);
        /*Force Numbers */
        tokenizeNumber(0, digitsBuffer);
        SSD1306_setDigits_16x32(digitsBuffer, &forceDigits);
        /* Units */
        SSD1306_setLetters(kgText_bitmaps, &unitLetters);
        
        uint16_t forceVal = 0;
        uint16_t forceVal_prev = 0;
        
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
                else if ('0' == readVal) {
                    printLn(&usb, "Blank screen");
                    SSD1306_clearScreen();
                }
                else if (' ' == readVal) {
                    forceVal += (loopCount & 0xFF) >> 2;
                    printLn(&usb, "Force  Val:%i", forceVal);
                    if(forceVal > 300){
                     forceVal = 0;   
                    }
                }
            }
            /* Update the force */
            if (forceVal !=forceVal_prev){
                tokenizeNumber(forceVal, digitsBuffer);
                SSD1306_setDigits_16x32(digitsBuffer, &forceDigits);
                forceVal_prev = forceVal;
            }
            /* Use loop count + keyboard input as form of randomness */
            loopCount++;
        }
    /* End MJL_DEBUG_DRAW_UI */
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
    rgb.fn_pin_R_Write = pin_LED_R_Write;
    rgb.fn_pin_G_Write = pin_LED_G_Write;
    rgb.fn_pin_B_Write = pin_LED_B_Write;
    RGB_Start(&rgb, RGB_ACTIVE_LOW);
    
    /* Start the UART */
    error |= uartPsoc_start(&usb, 115200);
    /* Start the SPI component */
    error |= spiPsoc_start(&spi);
    /* start the OLED */
    SSD1306_start();
    
    /* Set the screen parameters */
    batteryIcon.origin_col = 0;
    batteryIcon.origin_row = 0;
    batteryIcon.size_cols=16;
    batteryIcon.size_rows=8;
    batteryIcon.repeat_num=1;
    batteryIcon.repeat_spacing=0;
    
    maxForceDigits.origin_col = 93;
    maxForceDigits.origin_row = 0;
    maxForceDigits.size_cols=8;
    maxForceDigits.size_rows=16;
    maxForceDigits.repeat_num=4;
    maxForceDigits.repeat_spacing=1;
    
    forceDigits.origin_col = 19;
    forceDigits.origin_row = 8;
    forceDigits.size_cols=16;
    forceDigits.size_rows=32;
    forceDigits.repeat_num=4;
    forceDigits.repeat_spacing=2;
    
    exerciseDigit.origin_col = 0;
    exerciseDigit.origin_row = 16;
    exerciseDigit.size_cols=12;
    exerciseDigit.size_rows=24;
    exerciseDigit.repeat_num=1;
    exerciseDigit.repeat_spacing=0;
    
    maxLetters.origin_col = 102;
    maxLetters.origin_row = 24;
    maxLetters.size_cols=8;
    maxLetters.size_rows=16;
    maxLetters.repeat_num=3;
    maxLetters.repeat_spacing=1;

    historyIcon.origin_col = 0;
    historyIcon.origin_row = 40;
    historyIcon.size_cols=16;
    historyIcon.size_rows=24;
    historyIcon.repeat_num=1;
    historyIcon.repeat_spacing=0;
    
    resetIcon.origin_col = 112;
    resetIcon.origin_row = 40;
    resetIcon.size_cols=16;
    resetIcon.size_rows=24;
    resetIcon.repeat_num=1;
    resetIcon.repeat_spacing=0;
    
    unitLetters.origin_col = 72;
    unitLetters.origin_row = 48;
    unitLetters.size_cols=8;
    unitLetters.size_rows=16;
    unitLetters.repeat_num=2;
    unitLetters.repeat_spacing=1;
    
    fullWindow.colStart = 0;
    fullWindow.colEnd = 127;
    fullWindow.pageStart = 0;
    fullWindow.pageEnd = 7;

    
    return error;
}

/*******************************************************************************
* Function Name: SSD1306_start()
********************************************************************************
* \brief
*   Initializes the OLED
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_start(void) {
    uint32_t error = 0;
    /* Hardware Reset */
    pin_OLED_RESET_Write(SSD1306_RESET_ASSERT);
    CyDelayUs(10);
    pin_OLED_RESET_Write(SSD1306_RESET_DISASSERT);
    CyDelay(1);
    /* Enable the display */
    #define SSD_START_ARRAY_LEN 5
    uint8_t startCommands[SSD_START_ARRAY_LEN] = {
        SSD1306_CMD_CHARGE_PUMP, SSD1306_CMD_CHARGE_PUMP_ON,  /* Enable the charge pump */
        SSD1306_CMD_ADDRESS_MODE, SSD1306_CMD_ADDRESS_MODE_HORIZONTAL, /* Set to horizontal addressing */
        SSD1306_CMD_ON /* Wakeup from sleep */
    };
    error |= SSD1306_writeCommandArray(startCommands, SSD_START_ARRAY_LEN);
//    error |= SSD1306_clearScreen();

    return error;
}

/*******************************************************************************
* Function Name: SSD1306_writeCommandArray()
********************************************************************************
* \brief
*   Turns the screen on
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_writeCommandArray(uint8_t * cmdArray, uint8_t len) {
    uint32_t error = 0;
    /* Set the Command line low */
    pin_OLED_DC_Write(SSD1306_DC_COMMAND);
    /* Write array and wait until complete */
    spi_writeArrayBlocking(SPI_ID_OLED, cmdArray, len);
    /* Switch back to Data mode */
    pin_OLED_DC_Write(SSD1306_DC_DATA);
        
    return error;
}

/*******************************************************************************
* Function Name: SSD1306_writeDataArray()
********************************************************************************
* \brief
*   Turns the screen on
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_writeDataArray(const uint8_t * dataArray, uint16_t len) {
    uint32_t error = 0;
    /* Assert that data is being written */
    pin_OLED_DC_Write(SSD1306_DC_DATA);
    /* Write array and wait until complete */
    spi_writeArrayBlocking(SPI_ID_OLED, dataArray, len);
        
    return error;
}




/*******************************************************************************
* Function Name: SSD1306_setWindow()
********************************************************************************
* \brief
*   Sets the drawing page and columns
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_setWindow(display_window_s *const window) {
    uint32_t error = 0;
    #define WINDOW_BUFFER_LEN 6
    uint8_t dataArray[WINDOW_BUFFER_LEN] = {
        SSD1306_CMD_COLUMN_ADDR, window->colStart, window->colEnd,
        SSD1306_CMD_PAGE_ADDR, window->pageStart, window->pageEnd
    };
    SSD1306_writeCommandArray(dataArray, WINDOW_BUFFER_LEN);
    return error;
}

/*******************************************************************************
* Function Name: SSD1306_windowFromPos()
********************************************************************************
* \brief
*   Calculate a window given a position
*
* \param pos [in]
*   Inputted position
* 
* \param instanceNum [in]
*   Instance number for repeating block. Zero indexed
* 
* \param window [out]
*   Calculated windown
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_windowFromPos(display_position_s *const pos, uint8_t instanceNum, display_window_s *const window) {
    uint32_t error = 0;
    /* Calculate the window */
    uint8_t pageStart = pos->origin_row/SSD1306_PAGE_HEIGHT;
    uint8_t pageEnd = pageStart + (pos->size_rows/SSD1306_PAGE_HEIGHT) - 1;
    /* Linear repeat pattern */
    uint8_t colStart = pos->origin_col + instanceNum*pos->size_cols;
    /* Add spacing */
    colStart+= pos->repeat_spacing*instanceNum;
    uint8_t colEnd = colStart + pos->size_cols - 1;

    /* Set the window */
    window->colStart = colStart;
    window->colEnd = colEnd;
    window->pageStart = pageStart;
    window->pageEnd = pageEnd;
    return error;
}



/*******************************************************************************
* Function Name: SSD1306_drawDigit_8x16()
********************************************************************************
* \brief
*   Draws a given digit
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_drawDigit_8x16(uint8_t num){
    uint32_t error = 0;
    /* Only Digits */
    if(num >9){
        error = COMMS_ERROR_VAL;
    }
    if(!error) {
        SSD1306_writeDataArray(digits_8x16[num], UI_TEXT_8x16_LEN);
    }
    return error;
}

/*******************************************************************************
* Function Name: SSD1306_clearScreen()
********************************************************************************
* \brief
*   Clear the screen
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_clearScreen(){
    uint32_t error = 0;
    /* Make an empty page */
    uint8_t blankPage[SSD1306_NUM_COLS];
    memset(blankPage,0,SSD1306_NUM_COLS);
    SSD1306_setWindow(&fullWindow);
    uint8_t i;
    /* Write all the pages */
    for(i=0; i < SSD1306_NUM_PAGE; i++){
        error|= SSD1306_writeDataArray(blankPage,SSD1306_NUM_COLS);
        if(error){
            printLn(&usb, "Clear error %i",error);
            break;
        }
    }
    return error;
}


/*******************************************************************************
* Function Name: SSD1306_setDigits()
********************************************************************************
* \brief
*   Set a series of digits at a position
*
* \param digits [in]
*  Pointer to array that contains the information for each digit to set 
* 
* \param pos [in]
* Position object of the digits
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_setDigits(uint8_t *digits, display_position_s *const pos){
    uint32_t error = 0;
    /* Only support whole pages right now */
    if(pos->origin_row % SSD1306_PAGE_HEIGHT != 0){
        error = 1;
        printLn(&usb, "Non integer page row location %i", pos->origin_row);
    }
        
    if(!error) {
        uint8_t i; 
        bool shouldDigitRender = false;
        for(i=0; i < pos->repeat_num; i++){
            /* Don't render leading zeros */
            if(shouldDigitRender || digits[i] > 0 ){
                shouldDigitRender = true;
            }
            /* Render last 0 */
            else if(i == pos->repeat_num -1){}
            else {continue;}
            /* Calculate the window */
            display_window_s window;
            SSD1306_windowFromPos(pos, i, &window);
            SSD1306_setWindow(&window); 
            /* draw the digit */
            SSD1306_drawDigit_8x16(digits[i]);
        }
    }
    return error;
}

/*******************************************************************************
* Function Name: SSD1306_setDigits_16x32()
********************************************************************************
* \brief
*   Set a series of digits at a position
*
* \param digits [in]
*  Pointer to array that contains the information for each digit to set 
* 
* \param pos [in]
* Position object of the digits
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_setDigits_16x32(uint8_t *digits, display_position_s *const pos){
    uint32_t error = 0;
    /* Only support whole pages right now */
    if(pos->origin_row % SSD1306_PAGE_HEIGHT != 0){
        error = 1;
        printLn(&usb, "Non integer page row location %i", pos->origin_row);
    }
        
    if(!error) {
        uint8_t i; 
        bool shouldDigitRender = false;
        for(i=0; i < pos->repeat_num; i++){
            /* Don't render leading zeros */
            if(shouldDigitRender || digits[i] > 0 ){
                shouldDigitRender = true;
            }
            /* Render last 0 */
            else if(i == pos->repeat_num -1){}
            else {
                continue;
            }
            /* Calculate the digit */
            uint8_t newLetter[DISPLAY_LEN_16x32];
            SSD1306_create16x32(digits_8x16[digits[i]], newLetter);
            /* Calculate the window */
            display_window_s window;
            SSD1306_windowFromPos(pos, i, &window);
            SSD1306_setWindow(&window); 
            /* draw the digit */
            SSD1306_writeDataArray(newLetter, DISPLAY_LEN_16x32);
        }
    }
    return error;
}

/*******************************************************************************
* Function Name: SSD1306_setLetters()
********************************************************************************
* \brief
*   Set a series of letters at a position
*
* \param letters [in]
*  Pointer to array that contains the information for each letter to set 
* 
* \param pos [in]
* Position object of the letters
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_setLetters(const uint8_t **letters, display_position_s *const pos){
    uint32_t error = 0;
    /* Only support whole pages right now */
    if(pos->origin_row % SSD1306_PAGE_HEIGHT != 0){
        error = 1;
        printLn(&usb, "Non integer page row location %i", pos->origin_row);
    }
        
    if(!error) {
        uint8_t i; 
        for(i=0; i < pos->repeat_num; i++){
            /* Calculate the window */
            display_window_s window;
            SSD1306_windowFromPos(pos, i, &window);
            SSD1306_setWindow(&window); 
            /* draw the digit */
            SSD1306_writeDataArray(letters[i], UI_TEXT_8x16_LEN); 
        }
    }
    return error;
}


/*******************************************************************************
* Function Name: SSD1306_create16x32()
********************************************************************************
* \brief
*   Scale up an 8x16 object to create a 16x32
*
* \param letters [in]
*   
* 
* \param pos [in]
* 
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_create16x32(const uint8_t * inObject, uint8_t * outObject){
    uint32_t error = 0;
    
    uint8_t dataIndex;
    for(dataIndex=0;dataIndex<16;dataIndex++){
        uint8_t lowNibble  = (inObject[dataIndex] & 0x0F);
        uint8_t highNibble  = (inObject[dataIndex] >> 4) & 0x0F;
        
        uint8_t bitIndex;
        uint8_t newHighByte = 0;
        uint8_t newLowByte = 0;
        /* Iterate through each bit in the nibble*/
        for(bitIndex=0; bitIndex<4; bitIndex++){
            /* High byte */
            bool bit = (highNibble >> bitIndex) & 0x01;
            if(bit) {
                /* Double in Rows */
                newHighByte |= 0b11 << (bitIndex *2);
            }
            /* low byte */
            bit = (lowNibble >> bitIndex) & 0x01;
            if(bit) {
                /* Double in Rows */
                newLowByte |= (0b11 << (bitIndex *2));
            }
        }  
        // TODO: refactor these out
        #define INOBJECT_NUM_COLS 8
        #define OUTOBJECT_NUM_COLS 16
        /* Calculate Indices */
        uint8_t l1 = dataIndex*2 +OUTOBJECT_NUM_COLS*(dataIndex/INOBJECT_NUM_COLS);
        uint8_t l2 = l1 +1;
        uint8_t h1 = dataIndex*2+ OUTOBJECT_NUM_COLS*((dataIndex/INOBJECT_NUM_COLS)+1);
        uint8_t h2 = h1+1;
        /* Double write Cols */
        outObject[h1] = newHighByte;
        outObject[h2] = newHighByte;
        outObject[l1] = newLowByte;
        outObject[l2] = newLowByte;
//        printLn(&usb, "i:%i, B:0x%x, hNib:0x%x, lNib:0x%x,  H:0x%x, L:0x%x, h:%i,%i, l:%i,%i", i,inObject[i],highNibble, lowNibble, newHighByte,newLowByte,h1, h2, l1, l2);
    }
    
    
    return error;
}



/*******************************************************************************
* Function Name: spi_writeArrayBlocking()
********************************************************************************
* \brief
*   Write an array of data via SPI 
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t spi_writeArrayBlocking(uint8_t slaveId, const uint8_t * cmdArray, uint16_t len) {
    uint32_t error = 0;
    spiSlaveSelect_Write(slaveId);
    uint8_t i;
    uint16_t count = 0;
    /* Place the array into the TX buffer */
    for(i=0; i<len; i++){
        /* Ensure room in the TX buffer */
        bool isRoomInBuffer = false;
        count = 0;
        while(false == isRoomInBuffer){
            isRoomInBuffer = SPIM_STS_TX_FIFO_NOT_FULL & SPIM_ReadTxStatus();
            if(++count == 0){
                printLn(&usb, "SPI 'Load TX Buffer' Timeout");
                error = 1;
                break;
            }    
        }
        /* Exit on error */
        if(error){
            break;
        }
        /* Add TX data to SPI Buffer */
        SPIM_WriteTxData(cmdArray[i]);  
    }
    if(!error){
        /* Wait for the write buffer to complete */
        bool isSpiDone = false;
        count = 0;
        while(false == isSpiDone){
            isSpiDone = SPIM_STS_SPI_DONE & SPIM_ReadTxStatus();
            if(++count == 0){
                error=2;
                printLn(&usb, "SPI 'Done' Timeout");
                break;
            }    
        }   
    }
    return error;
}

/*******************************************************************************
* Function Name: tokenizeNumber()
********************************************************************************
* \brief
*   Returns individual digits from a 4 digit number
*
* \param val [in]
*   Value to tokenize
*
* \param outArray[out]
*  Array to place resulting digits. Must be at least 4 bytes long
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t tokenizeNumber(uint16_t val, uint8_t *outArray){
    uint32_t error = 0;
    /* Calculate individidual digits */
    uint8_t digit_1000s = val/1000;
    val-= 1000 * digit_1000s;
    uint8_t digit_100s = val/100;
    val-= 100*digit_100s;
    uint8_t digit_10s = val/ 10;
    val-= 10*digit_10s;
    uint8_t digit_1s = val;
    /* Write to the array */
    outArray[0] = digit_1000s;
    outArray[1] = digit_100s;
    outArray[2] = digit_10s;
    outArray[3] = digit_1s;
    
    return error;
}

/* [] END OF FILE */
