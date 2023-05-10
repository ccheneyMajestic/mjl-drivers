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
#include "OLED_SSD1306.h"
#include "mjl_font.h"


/* ####################### BEGIN PROGRAM CONFIGURATION ###################### */

#define MJL_DEBUG
/* ---------------- DEBUG CASE ----------------
* Uncomment ONE of the following
* Debugging will only occur when MJL_DEBUG is defined
*/
#ifdef MJL_DEBUG
//    #define MJL_DEBUG_LEDS           /* Test the onboard RGB LED */
//    #define MJL_DEBUG_UART           /* Test the UART Connection */
    #define MJL_DEBUG_SCREEN_START      /* Start the screen */ 
//    #define MJL_DEBUG_SCREEN           /* Turn the entire screen on */
//    #define MJL_DEBUG_SCREEN_NO_UART           /* Turn the entire screen on */
//    #define MJL_DEBUG_DRAW_UI          /* Draw the UI */
#endif
/* -------------- END DEBUG CASE --------------  */
   


/* ############################# BEGIN PROGRAM ############################## */
#define DELAY 500
#define LEN_ROW 128

#define SL_SPI_ID_DISPLAY   (0) /* SPI Slave ID of the Display */
#define SL_SPI_ID_INA       (1) /* SPI Slave ID of the Instrumentation Amp */
#define SL_SPI_ID_FLASH     (2) /* SPI Slave ID of the Flash */
#define SPI_CS_ACTIVE       (0) /* Value to assert an active slave) */
#define SPI_CS_INACTIVE     (1) /* Value to disassert a slave) */
#define ERROR_SHIFT_VAL 1
#define ERROR_TIMEOUT 2

/* Function declarations */ 
uint32_t initHardware(void); 
uint32_t spi_scbWriteArrayBlocking(uint8_t slaveId, uint8_t * cmdArray, uint16_t len);
uint32_t spi_assertSlave(uint8_t slaveId);
void spi_disassertSlave(void);
void pin_oled_reset_write_dummy(uint8_t val);

/* Global Variables */
RGB_S rgb;
COMMS_UART_S usb;
COMMS_SPI_S spi;
ssd1306_state_s display;

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
    uint32_t error  = initHardware();
    printHeader(&usb, "MJL Driver Abstraction Main");
    printErrorStatus(&usb, "Initialize", error, 0);

    
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
    #elif defined MJL_DEBUG_SCREEN_START      
        /* Start the screen */ 
        printHeader(&usb, "MJL_DEBUG_SCREEN_START");
        CyDelay(10);
        print(&usb, "\r\n");
        RGB_Write(&rgb, RGB_Red);
        
        uint32_t error = SSD1306_start(&display);
        printErrorStatus(&usb, "Start display", error, 0);

        bool isDisplayDataShown = true;
        for(;;) {
            /* Handle UART */
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
            /* Toggle between all on and all off */
            CyDelay(1000);
            RGB_B_Toggle(&rgb);
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
//            SSD1306_writeCommandArray(&display, &cmdWord, 1);
        }
    /* End MJL_DEBUG_SCREEN_START */    
    #elif defined MJL_DEBUG_SCREEN           
        /* Turn the entire screen on */
        printHeader(&usb, "MJL_DEBUG_SCREEN");
        CyDelay(10);
        print(&usb, "\r\n");
        RGB_Write(&rgb, RGB_Blue);
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
                    RGB_R_Toggle(&rgb);
                }
                else if('1' == readVal) {
                    printLn(&usb, "Writing display row");
                    RGB_G_Toggle(&rgb);
                    uint8_t dataArray[LEN_ROW];
                    memset(dataArray, 0xFF, LEN_ROW);
                    SSD1306_writeDataArray(&display, dataArray, LEN_ROW);
                }
                else if('2' == readVal) {
                    printLn(&usb, "Clearing Display data");
                    RGB_G_Toggle(&rgb);
                    SSD1306_clearScreen(&display);
                }
            }
        }
    /* End MJL_DEBUG_SCREEN_ON */
    #elif defined MJL_DEBUG_SCREEN_NO_UART           
        /* Turn the entire screen on */
        RGB_Write(&rgb, RGB_Blue);
        uint32_t error = SSD1306_clearScreen(&display);
        if(error){
            RGB_Write(&rgb, RGB_Green);
        }
            
        
        for(;;){
            
        }
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
        uint8_t digitsBuffer[3];
        uint16_t loopCount = 0;
        
        /**** Drawing ****/
        SSD1306_clearScreen(&display);
        /* Battery Icon */
        windowFromPos(&batteryIcon,0,&activeWindowBuffer);
        SSD1306_setWindow(&display, &activeWindowBuffer);
        SSD1306_writeDataArray(&display, battery_1bar, UI_ICON_BATTERY_LEN);
        /* History Icon */
        windowFromPos(&historyIcon,0,&activeWindowBuffer);
        SSD1306_setWindow(&display, &activeWindowBuffer);
        SSD1306_writeDataArray(&display, historyOutline, UI_ICON_HISTORY_LEN);
        /* Reset Icon */
        windowFromPos(&resetIcon,0,&activeWindowBuffer);
        SSD1306_setWindow(&display, &activeWindowBuffer);
        SSD1306_writeDataArray(&display, resetOutline, UI_ICON_RESET_LEN);
        /* Max Force */
        tokenizeNumber(130, digitsBuffer);
        SSD1306_setDigits(&display, digitsBuffer, &maxForceDigits);
        /* Max text */
        SSD1306_setLetters(&display, maxText_bitmaps, &maxLetters);
        /*Force Numbers */
        tokenizeNumber(0, digitsBuffer);
        SSD1306_setDigits_16x32(&display, digitsBuffer, &forceDigits);
        /* Units */
        SSD1306_setLetters(&display, kgText_bitmaps, &unitLetters);
        
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
                    SSD1306_clearScreen(&display);
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
                SSD1306_setDigits_16x32(&display, digitsBuffer, &forceDigits);
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
    SPI_Start();
//    error |= spiPsoc_start(&spi);
    /* start the OLED */
    ssd1306_cfg_s display_cfg;
    display_cfg.fn_spi_writeArrayBlocking = spi_scbWriteArrayBlocking;
    display_cfg.fn_pin_reset_write = pin_oled_reset_write_dummy;
    display_cfg.fn_pin_dataCommand_write = pin_OLED_DC_Write;
    display_cfg.fn_delayMs = CyDelay;
    display_cfg.fullWindow.colStart = 0;
    display_cfg.fullWindow.colEnd = 127;
    display_cfg.fullWindow.pageStart = 0;
    display_cfg.fullWindow.pageEnd = 7;
    display_cfg.spi_slaveId = 0;
    error |= SSD1306_init(&display, &display_cfg);
    error |= SSD1306_start(&display);
    
    /* Set the screen params */
    batteryIcon.origin_col = 0;
    batteryIcon.origin_row = 0;
    batteryIcon.size_cols=16;
    batteryIcon.size_rows=8;
    batteryIcon.repeat_num=1;
    batteryIcon.repeat_spacing=0;
    
    maxForceDigits.origin_col = 102;
    maxForceDigits.origin_row = 0;
    maxForceDigits.size_cols=8;
    maxForceDigits.size_rows=16;
    maxForceDigits.repeat_num=3;
    maxForceDigits.repeat_spacing=1;
    
    forceDigits.origin_col = 37;
    forceDigits.origin_row = 8;
    forceDigits.size_cols=16;
    forceDigits.size_rows=32;
    forceDigits.repeat_num=3;
    forceDigits.repeat_spacing=2;
    
    exerciseDigit.origin_col = 0;
    exerciseDigit.origin_row = 16;
    exerciseDigit.size_cols=8;
    exerciseDigit.size_rows=16;
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
    
    return error;
}


/*******************************************************************************
* Function Name: spi_scbWriteArrayBlocking()
********************************************************************************
* \brief
*   Write an array of data via SPI using the SCB api. This is a blocking function
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t spi_scbWriteArrayBlocking(uint8_t slaveId, uint8_t * cmdArray, uint16_t len) {
    uint32_t error = 0;
    /* Bit bang CS line */
    error |= spi_assertSlave(slaveId);
    /* Place the array into the TX buffer */
    SPI_SpiUartPutArray(cmdArray, len);
    if(!error){
        /* Wait for the write buffer to complete */
        uint16_t count = 0;
        /* Wait until data has been shifted in */
        while(!SPI_SpiIsBusBusy()){
            if(++count == 0){
                error=ERROR_TIMEOUT;
                // printLn(&usb, "SPI 'Done' Timeout");
                break;
            }    
        }
        /* Wait until data has been shifted out */
        if(!error){
            count = 0;
            while(SPI_SpiIsBusBusy()){
                if(++count == 0){
                    error=ERROR_TIMEOUT;
                    // printLn(&usb, "SPI 'Done' Timeout");
                    break;
                }    
            }   
        }
    }
    /* Bit bang CS line */
    spi_disassertSlave();
    
    return error;
}

/*******************************************************************************
* Function Name: spi_assertSlave()
********************************************************************************
* \brief
*  Bit banged - Set the active slave for the SPI
*
* \return
*  None
*******************************************************************************/
uint32_t spi_assertSlave(uint8_t slaveId){
    uint32_t error = 0;
    /* OLED Display */
    if(slaveId == SL_SPI_ID_DISPLAY){
        pin_SPI_CS_DISP_Write(SPI_CS_ACTIVE);   
    }
    /* Instrumentation Amp */
    else if (slaveId == SL_SPI_ID_INA) {
        pin_SPI_CS_INA_Write(SPI_CS_ACTIVE);   
    }
    /* Flash Memory */
    else if (slaveId == SL_SPI_ID_FLASH) {
        pin_SPI_CS_FLASH_Write(SPI_CS_ACTIVE);   
    }
    /* Invalid ID */
    else{
        error |= ERROR_SHIFT_VAL;   
    }
    return error;
}


/*******************************************************************************
* Function Name: spi_disassertSlave()
********************************************************************************
* \brief
*  Bit banged - Remove the active slave
*
* \return
*  None
*******************************************************************************/
void spi_disassertSlave(void){
    pin_SPI_CS_DISP_Write(SPI_CS_INACTIVE);
    pin_SPI_CS_INA_Write(SPI_CS_INACTIVE);
    pin_SPI_CS_FLASH_Write(SPI_CS_INACTIVE);
}



/*******************************************************************************
* Function Name: pin_oled_reset_write_dummy()
********************************************************************************
* \brief
*   Stand in for controlling the reset pin,as it is tied to the AUX_EN pin 
*
* \return
*  None
*******************************************************************************/
void pin_oled_reset_write_dummy(uint8_t val){
    (void) val;
}

/* [] END OF FILE */
