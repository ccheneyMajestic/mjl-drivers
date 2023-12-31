/***************************************************************************
*                                Majestic Labs © 2023
* File: TFT_GC9107_H.h
* Workspace: MJL Driver Library 
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: Driver file for the GC9107 TFT Driver  
*
* 2023.09.11  - Document Created
********************************************************************************/
#include "TFT_GC9107.h"
#include "TFT_hal_psoc4.h"
#include "project.h"

LCD_WINDOW_S window;
rgb_s color;


/*******************************************************************************
* Function Name: lcd_init()
********************************************************************************
* \brief
*   Start the LCD Screen
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t lcd_init(void) {
  uint32_t error = 0;
  // uart_println(&usb, "Initializing LCD hardware...");
  pin_LCD_3V3_Write(AUX_OFF);
  CyDelay(LCD_DELAY_RESET);
  pin_LCD_3V3_Write(AUX_ON);
  CyDelay(LCD_DELAY_RESET);
  PWM_LCD_Start();
  
  return error;
}


/*******************************************************************************
* Function Name: lcd_wakeup()
********************************************************************************
* \brief
*   Boots the LCD screen 
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t lcd_wakeup(void){
  uint32_t error = 0;
  // uart_println(&usb, "Waking LCD...");
  pin_LCD_DC_Write(LCD_DC_COMMAND);
  /*Turn sleep mode off  */ 
  lcd_writeCommand(LCD_CMD_SLEEPOUT);
  CyDelay(LCD_DELAY_WAKEUP);
  /* Set the Pixel format to 16 bits */
  uint8_t pixelMode = LCD_COLMOD_BITS_16;
  lcd_writeCommand_withParam(LCD_CMD_COLMOD, &pixelMode, 1);

  /* Set the column */
  window.start.x = 0;
  window.start.y = 0;
  window.end.x = LCD_PIXEL_HORZ-1;
  window.end.y = LCD_PIXEL_VERT-1;
  window.numCols = 1 + window.end.x - window.start.x;
  window.numRows = 1 + window.end.y - window.start.y;
  
  /* Write a solid color */
  colorName2Rgb(WHITE, &color, 0xFF);
  lcd_drawColor(&window, &color);
  
  /* Turn display to ON */
  lcd_writeCommand(LCD_CMD_DISPON);
  
  return error;
}


/*******************************************************************************
* Function Name: lcd_drawColor()
********************************************************************************
* \brief
*   Pack 16 bit pixel data 
* \param window
*   window for which to write the data 
* \param row
* RGB row information
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t lcd_drawColor(LCD_WINDOW_S *window, rgb_s * color){
  uint32_t error = 0;
  rgb_s row[window->numCols];
  for(uint16_t i=0; i<window->numCols; i++){
    row[i].r = color->r;
    row[i].g = color->g;
    row[i].b = color->b;
  }
  /* Write all the rows */
  lcd_writeRgb16_window(window, row);
  return error;
}


/*******************************************************************************
* Function Name: lcd_color2Pixel()
********************************************************************************
* \brief
*   Color name to value 

* \return
*  Error code of the operation
*******************************************************************************/
uint32_t colorName2Rgb(RGB_COLORS_T color, rgb_s * rgb, uint8_t intensity) {
  uint32_t error = 0;
  rgb->r = (color & COLOR_MASK_R) ? intensity : 0;
  rgb->g = (color & COLOR_MASK_G) ? intensity : 0;
  rgb->b = (color & COLOR_MASK_B) ? intensity : 0;
  return error;
}

/*******************************************************************************
* Function Name: lcd_writeRgb16_window()
********************************************************************************
* \brief
*   Pack 16 bit pixel data 
* \param window
*   window for which to write the data 
* \param row
* RGB row information
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t lcd_writeRgb16_window(LCD_WINDOW_S * window, rgb_s * row){
  uint32_t error = 0;
  lcd_setWindow(window);
  uint16_t lenData = window->numCols * 2;
  uint8_t rowData[lenData];
  for(uint16_t i=0; i<window->numCols; i++){
    /* Scale down from 8 bits and take complement */
    #define MASK_5_BIT (0x1F)
    #define MASK_6_BIT (0x3F)
    volatile uint8_t rRaw = row[i].r;
    volatile uint8_t gRaw = row[i].g;
    volatile uint8_t bRaw = row[i].b;
    
    uint8_t r = MASK_5_BIT - (rRaw >> 3);
    uint8_t g = MASK_6_BIT - (gRaw >> 2);
    uint8_t b = MASK_5_BIT - (bRaw >> 3);
    /* Pack into 16 bits B5-G6-R5 */
    volatile uint8_t MSB = ((b << 3) | (g >> 3)) & 0xFF;
    volatile uint8_t LSB = ((g << 5) | (r)) & 0xFF;
    rowData[2*i] = MSB;
    rowData[2*i+1] = LSB;
  }
  lcd_writeCommand(LCD_CMD_MEMWRITE);
  for(uint8_t row=0; row < window->numRows; row++){
    lcd_writeData(rowData, lenData);
  }
  return error;
}

/*******************************************************************************
* Function Name: lcd_setDisplayMode()
********************************************************************************
* \brief
*   Turn the display on or off
* 
* \param state
*   Next state to turn the device into 
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t lcd_setDisplayMode(bool state){
  uint32_t error = 0;
  uint8_t cmd = state ? LCD_CMD_DISPON : LCD_CMD_DISPOFF;
  error |= lcd_writeCommand(cmd);
  return error;
}
  
/*******************************************************************************
* Function Name: lcd_writeCommand()
********************************************************************************
* \brief
*   Write a specified command to the LCD Screen
* 
* \param cmd
*   Command word to write to Driver
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t lcd_writeCommand(uint8_t cmd){
  uint32_t error = 0;
  pin_LCD_DC_Write(LCD_DC_COMMAND);
  error |= hal_spi_scbWriteArrayBlocking(SPI_ID_LCD, &cmd, 1);
  return error;
}

/*******************************************************************************
* Function Name: lcd_writeCommand_withParam()
********************************************************************************
* \brief
*   Write a specified command to the LCD Screen
* 
* \param cmd
*   Command word to write to Driver
*
* \ param params
*   Pointer to array containing parameters
* \param len
*   number of params 
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t lcd_writeCommand_withParam(uint8_t cmd, const uint8_t * param, uint16_t len){
  uint32_t error = 0;
  pin_LCD_DC_Write(LCD_DC_COMMAND);
  error |= hal_spi_scbWriteArrayBlocking(SPI_ID_LCD, &cmd, 1);
  pin_LCD_DC_Write(LCD_DC_DATA);
  error |= hal_spi_scbWriteArrayBlocking(SPI_ID_LCD, param, len);
  return error;
}

/*******************************************************************************
* Function Name: lcd_writeData()
********************************************************************************
* \brief
*   Write a specified command to the LCD Screen
*
* \param data
*   Pointer to array containing data
*
* \param len
*   Length of data to write 
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t lcd_writeData(uint8_t * data, uint16_t len){
  uint32_t error = 0;
  pin_LCD_DC_Write(LCD_DC_DATA);
  error |= hal_spi_scbWriteArrayBlocking(SPI_ID_LCD, data, len);
  return error;
}



/*******************************************************************************
* Function Name: lcd_setWindow()
********************************************************************************
* \brief
*   Sets the window 
*
* \param window
*   Pointer to the window settings
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t lcd_setWindow(const LCD_WINDOW_S * window){
  uint32_t error = 0;
  /* Set the column */
  uint16_t colData16[2] = {window->start.x, window->end.x};
  uint8_t colData8[4];
  uint16_to_uint8(colData16, colData8, 2);
  lcd_writeCommand_withParam(LCD_CMD_COLSET, colData8, 4);
  /* Set the row */
  uint16_t rowData16[2] = {window->start.y, window->end.y};
  uint8_t rowData8[4];
  uint16_to_uint8(rowData16, rowData8, 2);
  lcd_writeCommand_withParam(LCD_CMD_ROWSET, rowData8, 4);
  return error;
}

/*******************************************************************************
* Function Name: uint16_to_uint8()
********************************************************************************
* \brief
*   Converts an array of uint16's to uint8's. Uint8 array must be twice the length
*
* \param in
*   Values to conver
* 
* \param out 
*   Created values
*
* \param len
*   Length of the input
*
* \return
*  None
*******************************************************************************/
void uint16_to_uint8(const uint16_t * in, uint8_t * out, uint16_t len){
  for(uint16_t i=0; i<len; i++){
    uint8_t offset = i * 2;
    out[offset] = (uint8_t) (in[i] >> 8);
    out[offset+1] = (uint8_t) (in[i] & 0xFF);
  }
}

/*******************************************************************************
* Function Name: lcd_write_all_white()
********************************************************************************
* \brief
*   Write all of the pixels to white
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t lcd_write_all_white(void) {
  uint32_t error = 0;
  
  
    /* Turn display to ON */
  #define WAKE_LEN  (1)
  uint8_t wakeArray[WAKE_LEN] = {LCD_CMD_DISPON};
  hal_spi_scbWriteArrayBlocking(SPI_ID_LCD, wakeArray, WAKE_LEN);
  
  return error;
}

// /* Default config struct */
// const MJL_TEMPLATE_CFG_S template_cfg_default = {
//   .hal_req_function = NULL,
//   .hal_opt_start = NULL,
//   .hal_opt_stop = NULL,
// };

// /*******************************************************************************
// * Function Name: template_init()
// ********************************************************************************
// * \brief
// *   Initializes the state struct from a configuration struct 
// *
// * \param state [in/out]
// * Pointer to the state struct
// * 
// * \param cfg [in]
// * Pointer to the configuration struct
// *
// * \return
// *  Error code of the operation
// *******************************************************************************/
// uint32_t template_init(MLJ_TEMPLATE_S *const state, MJL_TEMPLATE_CFG_S *const cfg){
//   uint32_t error = 0;
//   /* Verify required functions */
//   error |= (NULL == state) ? ERROR_POINTER : ERROR_NONE;
//   error |= (NULL == cfg) ? ERROR_POINTER : ERROR_NONE;
//   error |= (NULL == cfg->hal_req_function) ? ERROR_POINTER : ERROR_NONE;
//   /* external start is not required */
//   /* external stop is not required */
//   /* Valid Inputs */
//   if(!error) {
//     /* Copy params */
//     state->hal_req_function = cfg->hal_req_function;
//     state->hal_opt_start = cfg->hal_opt_start;
//     state->hal_opt_stop =  cfg->hal_opt_stop;
//     /* Mark as initialized */
//     state->_init = true;
//     state->_running = false;
//   }
//   if(error){state->_init=false;}
//   return error;
// }

// /*******************************************************************************
// * Function Name: template_start()
// ********************************************************************************
// * \brief
// *   Starts the Object and call the external start function, if present 
// *
// * \param state [in/out]
// * Pointer to the state struct
// *
// * \return
// *  Error code of the operation
// *******************************************************************************/
// uint32_t template_start(MLJ_TEMPLATE_S *const state){
//   uint32_t error = 0;
//   if(!state->_init){error|=ERROR_INIT;}
//   if(state->_running){error|=ERROR_RUNNING;}

//   if(!error){
//     /* Pre-mark as running */
//     state->_running = true;
//     /* Run the external start function if present  */
//     if(NULL != state->hal_opt_start){
//       error |= state->hal_opt_start((MLJ_TEMPLATE_T *const) state);
//     }
//   }
//   return error;
// }

// /*******************************************************************************
// * Function Name: template_stop()
// ********************************************************************************
// * \brief
// *   Stops the object
// *
// * \param state [in/out]
// * Pointer to the state struct
// *
// * \return
// *  Error code of the operation
// *******************************************************************************/
// uint32_t template_stop(MLJ_TEMPLATE_S *const state){
//   uint32_t error = 0;
//   if(!state->_init){error|=ERROR_INIT;}
//   if(!state->_running){error|=ERROR_STOPPED;}

//   if(!error){
//     /* Run the external stop function if present  */
//     if(NULL != state->hal_opt_stop){
//       error |= state->hal_opt_stop((MLJ_TEMPLATE_T *const) state);
//     }
//     state->_running = false;
//   }
//   return error;
// }
/* [] END OF FILE */