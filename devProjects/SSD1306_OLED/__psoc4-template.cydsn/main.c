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


/* ####################### BEGIN PROGRAM CONFIGURATION ###################### */

#define IMPACT_DEBUG
/* ---------------- DEBUG CASE ----------------
* Uncomment ONE of the following
* Debugging will only occur when IMPACT_DEBUG is defined
*/
#ifdef IMPACT_DEBUG
//    #define IMPACT_DEBUG_LEDS           /* Test the onboard RGB LED */
    #define IMPACT_DEBUG_UART           /* Test the UART Connection */
#endif
/* -------------- END DEBUG CASE --------------  */
   


/* ############################# BEGIN PROGRAM ############################## */
#define DELAY 500

/* Function declarations */ 
uint32_t initHardware(void); 

/* Global Variables */
RGB_S rgb;
COMMS_UART_S usb;


/* Main Program */
#ifndef IMPACT_DEBUG
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
#endif /* ifndef IMPACT_DEBUG*/
/* End Main Program */


/* ############################ BEGIN DEBUGGER ############################## */
#ifdef IMPACT_DEBUG
/*******************************************************************************
* DEBUGGER: main()
********************************************************************************
* Summary:
*   Debugging function for the PCB hardware
*******************************************************************************/
int main(void){
    #warning "IMPACT_DEBUG is enabled"

    initHardware();
    
    /* Test Cases */
    #ifdef IMPACT_DEBUG_LEDS
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
        
    /* End IMPACT_DEBUG_LEDS */
    #elif defined IMPACT_DEBUG_UART           
        /* Test the UART Connection */
        printHeader(&usb, "IMPACT_DEBUG_UART");
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
    /* End IMPACT_DEBUG_5V */    
    #endif /* End IMPACT_DEBUG_<case> */
    /* Fall through Infinite loop */
    for(;;){}
}
#endif /* IMPACT_DEBUG */


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
    uartPsoc_start(&usb, 115200);
    
    return error;
}

/* [] END OF FILE */
