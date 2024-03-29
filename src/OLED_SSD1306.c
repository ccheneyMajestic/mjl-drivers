/***************************************************************************
*                             Majestic Labs © 2023
*
* File: OLED_SSD1306.h
* Workspace: MJL Driver Library 
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: Driver library for the SSD1306 OLED Display 128x64
*   
*
* 2023.02.18  - Document Created
********************************************************************************/
#include "OLED_SSD1306.h"
#include "mjl_errors.h"
#include <string.h>
#include "mjl_font.h"

/*******************************************************************************
* Function Name: SSD1306_init()
********************************************************************************
* \brief
*   Initializes the display struct
*
* \param state [in/out]
*  State structure of the display to be initialized
* 
* \param cfg  [in]
*   Structure that contains the configuration settings 
* 
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_init(ssd1306_state_s *const state, ssd1306_cfg_s *const cfg) {
    uint32_t error = 0;
    /* verify functions*/
    error |= (cfg->fn_spi_writeArrayBlocking == NULL) ? ERROR_POINTER : ERROR_NONE;
    error |= (cfg->fn_pin_reset_write == NULL) ? ERROR_POINTER : ERROR_NONE;
    error |= (cfg->fn_pin_dataCommand_write == NULL) ? ERROR_POINTER : ERROR_NONE;
    error |= (cfg->fn_delayUs == NULL) ? ERROR_POINTER : ERROR_NONE;
    /* Verify values */
    error |= (cfg->fullWindow.colEnd == 0) ? ERROR_VAL : ERROR_NONE;
    error |= (cfg->fullWindow.pageEnd == 0) ? ERROR_VAL : ERROR_NONE;

    if(!error) {
      /* Marked struct as initialzied */
      state->_isInitialized = true;
      /* Copy pointers from cfg */
      state->fn_spi_writeArrayBlocking = cfg->fn_spi_writeArrayBlocking;
      state->fn_pin_reset_write = cfg->fn_pin_reset_write;
      state->fn_pin_dataCommand_write = cfg->fn_pin_dataCommand_write;
      state->fn_delayUs = cfg->fn_delayUs;
      /* Copy Value from cfg */
      state->fullWindow.colStart  = cfg->fullWindow.colStart;
      state->fullWindow.colEnd    = cfg->fullWindow.colEnd;
      state->fullWindow.pageStart = cfg->fullWindow.pageStart;
      state->fullWindow.pageEnd   = cfg->fullWindow.pageEnd;
      state->spi_slaveId = cfg->spi_slaveId;
      /* Set default values */
      state->output = 0;
      state->error = 0;
      state->state_current = SSD1306_STATE_OFF;
      state->state_next = SSD1306_STATE_OFF;
      state->state_previous = SSD1306_STATE_OFF;
      state->state_requested = SSD1306_STATE_OFF;
      state->addressMode = SSD1306_ADDRESSING_PAGE;
    }
    else {
      state->_isInitialized = false;
      state->error = error;
    }
    return error;
}

/*******************************************************************************
* Function Name: SSD1306_start()
********************************************************************************
* \brief
*   Initializes the OLED. 
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_start(ssd1306_state_s *const state){
  uint32_t error = 0;
  if(!state->_isInitialized){error|=ERROR_INIT;}
  if(!error){
    /* Hardware Reset */
    state->fn_pin_reset_write(SSD1306_RESET_ASSERT);
    state->fn_pin_dataCommand_write(SSD1306_DC_DATA);
    state->fn_delayUs(SSD1306_DELAY_US_RESET);
    state->fn_pin_reset_write(SSD1306_RESET_DISASSERT);
    state->fn_delayUs(SSD1306_DELAY_US_RESET);
    #define SSD_START_ARRAY_LEN 5
    uint8_t startCommands[SSD_START_ARRAY_LEN] = {
      SSD1306_CMD_CHARGE_PUMP, SSD1306_CMD_CHARGE_PUMP_ON,  /* Enable the charge pump */
      SSD1306_CMD_ADDRESS_MODE, SSD1306_CMD_ADDRESS_MODE_HORIZONTAL, /* Set to horizontal addressing */
      SSD1306_CMD_ON /* Wakeup from sleep */
    };
    error |= SSD1306_writeCommandArray(state, startCommands, SSD_START_ARRAY_LEN);
    if(!error){
      error |= SSD1306_setWindow(state, &state->fullWindow);
      error |= SSD1306_clearScreen(state);
    }
  }
  return error;
}

/*******************************************************************************
* Function Name: SSD1306_stop()
********************************************************************************
* \brief
*   Initializes the OLED
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_stop(ssd1306_state_s *const state){
  uint32_t error = 0;
  if(!state->_isInitialized){error|=ERROR_INIT;}
  if(!error){
    #define SSD1306_STOP_ARRAY_LEN 3
    uint8_t stopCommands[SSD1306_STOP_ARRAY_LEN] ={
      SSD1306_CMD_CHARGE_PUMP, SSD1306_CMD_CHARGE_PUMP_OFF, 
      SSD1306_CMD_SLEEP
    };
    error |= SSD1306_writeCommandArray(state, stopCommands, SSD1306_STOP_ARRAY_LEN);

    /* Hardware Reset */
    state->fn_pin_reset_write(SSD1306_RESET_DISASSERT);
    state->fn_pin_dataCommand_write(SSD1306_DC_COMMAND);
    state->fn_delayUs(SSD1306_DELAY_US_RESET);

  }
  return error;
}


/*******************************************************************************
* Function Name: SSD1306_writeCommandArray()
********************************************************************************
* \brief
*   Write a command (not screen data) to the display. This is a blocking function
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_writeCommandArray(ssd1306_state_s *const state, uint8_t * cmdArray, uint8_t len) {
  uint32_t error = 0;
  if(!state->_isInitialized){error|=ERROR_INIT;}
  if(!error) {
    /* Set the Command line low */
    state->fn_pin_dataCommand_write(SSD1306_DC_COMMAND);
    state->fn_delayUs(SSD1306_DELAY_US_DC);
    /* Write array and wait until complete */
    error |= state->fn_spi_writeArrayBlocking(state->spi_slaveId, cmdArray, len);
    /* Switch back to Data mode */
    state->fn_pin_dataCommand_write(SSD1306_DC_DATA);
  }        
  return error;
}

/*******************************************************************************
* Function Name: SSD1306_writeDataArray()
********************************************************************************
* \brief
*   Write screen data to the display
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_writeDataArray(ssd1306_state_s *const state, const uint8_t * dataArray, uint16_t len) {
  uint32_t error = 0;
  if(!state->_isInitialized){error|=ERROR_INIT;}
  if(!error) {
    /* Assert that data is being written */
    state->fn_pin_dataCommand_write(SSD1306_DC_DATA);
    state->fn_delayUs(SSD1306_DELAY_US_DC);
    /* Write array and wait until complete */
    error |= state->fn_spi_writeArrayBlocking(state->spi_slaveId, dataArray, len);
  }       
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
uint32_t SSD1306_setWindow(ssd1306_state_s *const state, display_window_s *const window) {
  uint32_t error = 0;
  if(!state->_isInitialized){error|=ERROR_INIT;}

  if(!error) {
    #define WINDOW_BUFFER_LEN 6
    uint8_t dataArray[WINDOW_BUFFER_LEN] = {
      SSD1306_CMD_COLUMN_ADDR, window->colStart, window->colEnd,
      SSD1306_CMD_PAGE_ADDR, window->pageStart, window->pageEnd
    };
    error |= SSD1306_writeCommandArray(state, dataArray, WINDOW_BUFFER_LEN);
  }
  return error;
}

/*******************************************************************************
* Function Name: SSD1306_setAddressingMode()
********************************************************************************
* \brief
*   Set the addressing mode
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_setAddressingMode(ssd1306_state_s *const state, ssd1306_addressing_mode_t mode){
  uint32_t error = 0;
  if(!state->_isInitialized){error|=ERROR_INIT;}
  if(mode > SSD1306_ADDRESSING_PAGE){error|=ERROR_VAL;}

  if(!error) {
    #define ADDRESS_MODE_LEN (2)
    uint8_t dataArray[ADDRESS_MODE_LEN] = {
      SSD1306_CMD_ADDRESS_MODE, mode
    };
    error |= SSD1306_writeCommandArray(state, dataArray, ADDRESS_MODE_LEN);
    if(!error){state->addressMode = mode;}
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
uint32_t SSD1306_clearScreen(ssd1306_state_s *const state){
  uint32_t error = 0;
  if(!state->_isInitialized){error|=ERROR_INIT;}

  if(!error) {
    /* Make an empty page */
    uint8_t blankPage[SSD1306_NUM_COLS];
    memset(blankPage, 0, SSD1306_NUM_COLS);
    error |= SSD1306_setWindow(state, &state->fullWindow);
    uint8_t i;
    /* Write all the pages */
    for(i=0; i < SSD1306_NUM_PAGE; i++){
      error|= SSD1306_writeDataArray(state, blankPage,SSD1306_NUM_COLS);
      if(error){
        // printLn(&usb, "Clear error %i",error);
        break;
      }
    }
  }
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
uint32_t SSD1306_drawDigit_8x16(ssd1306_state_s *const state, uint8_t num){
    uint32_t error = 0;
    /* Only Digits */
    if(num>9){error|=ERROR_VAL;}
    if(!error) {
      error |= SSD1306_writeDataArray(state, digits_8x16[num], UI_TEXT_8x16_LEN);
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
uint32_t SSD1306_setDigits(ssd1306_state_s *const state, uint8_t* digits, display_position_s *const pos){
    uint32_t error = 0;
    /* Only support whole pages right now */
    if(pos->origin_row % SSD1306_PAGE_HEIGHT != 0){
      error |= ERROR_VAL;
      // printLn(&usb, "Non integer page row location %i", pos->origin_row);
    }
    if(!error) {
      uint8_t i; 
      bool shouldDigitRender = false;
      for(i=0; i < pos->repeat_num; i++){
        /* Don't render leading zeros */
        if(shouldDigitRender || digits[i] > 0){shouldDigitRender=true;}
        /* Render last 0 */
        else if(i == pos->repeat_num -1){}
        else {continue;}
        /* Calculate the window */
        display_window_s window;
        error |= windowFromPos(pos, i, &window);
        error |= SSD1306_setWindow(state, &window); 
        /* draw the digit */
        error |= SSD1306_drawDigit_8x16(state, digits[i]);
        if(error){break;}
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
uint32_t SSD1306_setDigits_16x32(ssd1306_state_s *const state, const uint8_t **digits, display_position_s *const pos){
    uint32_t error = 0;
    /* Only support whole pages right now */
    if(pos->origin_row % SSD1306_PAGE_HEIGHT != 0){
      error = ERROR_VAL;
    }
    if(!error) {
      for(uint8_t i=0; i< pos->repeat_num; i++){
        /* Calculate the digit */
        uint8_t newLetter[DISPLAY_LEN_16x32];
        error |= create16x32(digits[i], newLetter);
        /* Calculate the window */
        display_window_s window;
        error |= windowFromPos(pos, i, &window);
        error |= SSD1306_setWindow(state, &window); 
        error |= SSD1306_writeDataArray(state, newLetter, DISPLAY_LEN_16x32);
        if(error){break;}
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
* \param state [in/out]
*   Pointer to the state structure 
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
uint32_t SSD1306_setLetters(ssd1306_state_s *const state, const uint8_t **letters, display_position_s *const pos){
  uint32_t error = 0;
  /* Only support whole pages right now */
  if(pos->origin_row % SSD1306_PAGE_HEIGHT != 0){
    error = ERROR_VAL;
  }
  if(!error) {
    uint8_t i; 
    for(i=0; i < pos->repeat_num; i++){
      /* Calculate the window */
      display_window_s window;
      error |= windowFromPos(pos, i, &window);
      error |= SSD1306_setWindow(state, &window); 
      /* draw the digit */
      error |= SSD1306_writeDataArray(state, letters[i], UI_TEXT_8x16_LEN); 
      if(error){break;}
    }
  }
  return error;
}

/*******************************************************************************
* Function Name: SSD1306_renderString()
********************************************************************************
* \brief
*   Set a string in a giving position - all chars map to uppercase 
*
* \param state [in/out]
*   Pointer to the state structure 

* \param str [in]
*  Zero terminated string 
* 
* \param pos [in]
* Position object of the letters
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_renderString(ssd1306_state_s *const state, display_text_s *const text){
  uint32_t error = 0;
  /* Get the length of the string */
  uint8_t len = strlen(text->data);
  if(len > text->pos.repeat_num){error|=ERROR_PARAM;} /* Ensure string will fit*/
  if(text->pos.repeat_num > ROW_NUM_CHARS){error|=ERROR_VAL;}

  if(!error){
    const uint8_t* letters[ROW_NUM_CHARS];
    for(uint8_t i=0; i<len; i++){
      /* Set scaled up letter */
      uint8_t val = text->data[i];
      if(val>='A' && val<='Z'){letters[i] = alphabet_8x16[(val-ASCII_OFFSET_LETTER_CAP)];}
      else if(val>='a' && val<='z'){letters[i] = alphabet_8x16[(val-ASCII_OFFSET_LETTER_LOWER)];}
      else if(val>='0' && val<='9'){letters[i] = digits_8x16[(val-ASCII_OFFSET_DIGIT)];}
      else if(' '== val){letters[i] = specialChars_8x16[UI_CHARS_IDX_SPACE];}
      else if('-'== val){letters[i] = specialChars_8x16[UI_CHARS_IDX_DASH];}
      else{
        error|=ERROR_VAL;
        break;
      }
    }
    if(!error){
      /* Set large letter */
      if((16 == text->pos.size_cols) && (32 == text->pos.size_rows)){
        error |= SSD1306_setDigits_16x32(state, letters, &text->pos);
      }
      else{
        error|= SSD1306_setLetters(state, letters ,&text->pos);}
      }
  }
  return error;
}
/*******************************************************************************
* Function Name: SSD1306_setIcon()
********************************************************************************
* \brief
*   Sets an icon in a given position
*
* \param state [in/out]
*   Pointer to the state structure 

* \param icon [in]
*  Pointer to the icon strcuture 
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_setIcon(ssd1306_state_s *const state, display_icon_s *const icon){
  uint32_t error = 0;
  display_window_s window;
  error |= windowFromPos(&icon->pos, 0, &window);
  if(!error){error|=SSD1306_setWindow(state, &window);}
  if(!error){
    uint16_t len = (icon->pos.size_cols * icon->pos.size_rows) / SSD1306_PAGE_HEIGHT; 
    error|=SSD1306_writeDataArray(state, icon->data, len);
  }
  
  return error;
}

/*******************************************************************************
* Function Name: SSD1306_setIcon_inverse()
********************************************************************************
* \brief
*   Sets an inverted icon in a given position - only works with a 
*
* \param state [in/out]
*   Pointer to the state structure 

* \param icon [in]
*  Pointer to the icon strcuture 
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_setIcon_inverse(ssd1306_state_s *const state, display_icon_s *const icon){
  uint32_t error = 0;
  display_window_s window;
  error |= windowFromPos(&icon->pos, 0, &window);
  if(!error){error|=SSD1306_setWindow(state, &window);}
  if(!error){
    uint16_t len = (icon->pos.size_cols * icon->pos.size_rows) / SSD1306_PAGE_HEIGHT;
    uint8_t invert[len];
    for(uint8_t i=0; i<len;i++){
      invert[i]=(~icon->data[i]);
    } 
    error|=SSD1306_writeDataArray(state, invert, len);
  }
  
  return error;
}



/*******************************************************************************
* Function Name: SSD1306_clearIcon()
********************************************************************************
* \brief
*   Clears an icon in a given position
*
* \param state [in/out]
*   Pointer to the state structure 

* \param icon [in]
*  Icon to clear
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_clearIcon(ssd1306_state_s *const state, display_icon_s *const icon){
  uint32_t error = 0;
  display_window_s window;
  error |= windowFromPos(&icon->pos, 0, &window);
  if(!error){error|=SSD1306_setWindow(state, &window);}
  if(!error){
    uint16_t len = (icon->pos.size_cols * icon->pos.size_rows) / SSD1306_PAGE_HEIGHT; 
    // TODO: Hacky way of avoiding using heap 
    uint8_t blankPage[SSD1306_NUM_COLS];
    memset(blankPage, 0, SSD1306_NUM_COLS);
    error|=SSD1306_writeDataArray(state, blankPage, len);
  }
  
  return error;
}


/*******************************************************************************
* Function Name: windowFromPos()
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
uint32_t windowFromPos(display_position_s *const pos, uint8_t instanceNum, display_window_s *const window) {
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
* Function Name: create16x32()
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
uint32_t create16x32(const uint8_t * inObject, uint8_t * outObject){
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
//      printLn(&usb, "i:%i, B:0x%x, hNib:0x%x, lNib:0x%x,  H:0x%x, L:0x%x, h:%i,%i, l:%i,%i", i,inObject[i],highNibble, lowNibble, newHighByte,newLowByte,h1, h2, l1, l2);
  }
  return error;
}

/*******************************************************************************
* Function Name: tokenizeNumber()
********************************************************************************
* \brief
*   Returns individual digits from a 3 digit number
*
* \param val [in]
*   Value to tokenize
*
* \param outArray[out]
*  Array to place resulting digits. Must be at least 3 bytes long
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t tokenizeNumber(uint16_t val, uint8_t *outArray){
  uint32_t error = 0;
  /* Calculate individidual digits */
  uint8_t digit_1000s = val/1000;
  val-= 1000*digit_1000s;
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



/*******************************************************************************
* Function Name: display_line_init()
********************************************************************************
* \brief
*   Initialize the line object
*
* \param state [in/out]
*   Pointer to the graph
*
* \param cfg[in]
*  Configuration struct
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t display_line_init(display_line_s *const state, display_line_cfg_s *const cfg){
  uint32_t error = 0;
  if(cfg->colStart >= SSD1306_NUM_COLS) {error|=ERROR_PARAM;}
  if(cfg->rowStart >= SSD1306_NUM_ROWS) {error|=ERROR_PARAM;}
  if(cfg->numCol == 0) {error|=ERROR_PARAM;}
  if(cfg->numRow == 0) {error|=ERROR_PARAM;}
  /* Copy and perform math */
  if(!error){
    state->colStart = cfg->colStart;
    state->rowStart = cfg->rowStart;
    state->numCol = cfg->numCol;
    state->numRow = cfg->numRow;
    /* Derived params */
    state->rowEnd = state->rowStart + state->numRow - 1;
    state->colEnd = state->colStart + state->numCol - 1;
    state->pageStart = (state->rowStart / SSD1306_PAGE_HEIGHT);
    state->pageEnd = (state->rowEnd / SSD1306_PAGE_HEIGHT);
    state->numPage = 1 + (state->pageEnd - state->pageStart);
    /* Validate derived */
    if(state->rowEnd >= SSD1306_NUM_ROWS){error|=ERROR_VAL;}
    if(state->colEnd >= SSD1306_NUM_COLS){error|=ERROR_VAL;}
    if(state->pageEnd >= SSD1306_NUM_PAGE-1){error|=ERROR_VAL;} /* Can't be at bottom page*/
    /* Mark as successful*/
    if(!error){
      state->_isInit = true;
      state->shouldRender = true;
    }
  }
  if(error){state->_isInit=false;}
  return error;
}

/*******************************************************************************
* Function Name: SSD1306_drawLine()
********************************************************************************
* \brief
*   Draw a line on the screen
*
* \param state [in/out]
*   Pointer to the SSD state
*
* \param line[out]
*  Line to draw
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t SSD1306_drawLine(ssd1306_state_s *const state, display_line_s *const line){
  uint32_t error = 0;
  /* Set the window */
  display_window_s window;
  window.colStart = line->colStart;
  window.colEnd = line->colEnd;
  window.pageStart = line->pageStart;
  window.pageEnd = line->pageEnd;
  SSD1306_setWindow(state, &window);
  /* Generate the line */
  bool isHorizontalLine = (line->colStart != line->colEnd) && (line->rowStart == line->rowEnd);
  bool isVerticalLine   = (line->colStart == line->colEnd) && (line->rowStart != line->rowEnd);
  if(isHorizontalLine){
    uint8_t hLineData[line->numCol];
    memset(hLineData, 0x01, line->numCol);
    SSD1306_writeDataArray(state, hLineData, line->numCol);
  }
  else if(isVerticalLine){
    uint8_t vLineData[line->numPage];
    memset(vLineData, 0xFF, line->numPage-1);
    vLineData[line->numPage-1]= 0x03;
    SSD1306_writeDataArray(state, vLineData, line->numPage);
  }
  /* Diagonal lines not supported yet */
  else{error|=ERROR_INVALID;}
  return error;
}


/*******************************************************************************
* Function Name: display_graph_init()
********************************************************************************
* \brief
*   Initialize the graph object
*
* \param state [in/out]
*   Pointer to the graph
*
* \param cfg[in]
*  Configuration struct
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t display_graph_init(display_graph_s *const state, display_graph_cfg_s *const cfg){
  uint32_t error = 0;
  if(cfg->colStart >= SSD1306_NUM_COLS) {error|=ERROR_PARAM;}
  if(cfg->rowStart >= SSD1306_NUM_ROWS) {error|=ERROR_PARAM;}
  if(cfg->numCol == 0) {error|=ERROR_PARAM;}
  if(cfg->numRow == 0) {error|=ERROR_PARAM;}
  /* Only allow fully aligned pages right now*/
  if(0!=(cfg->rowStart%SSD1306_PAGE_HEIGHT)) {error|=ERROR_PARAM;}
  if(0!=(cfg->numRow%SSD1306_PAGE_HEIGHT)) {error|=ERROR_PARAM;}
  /* Ensure axes have room */
  if(cfg->colStart <= 2) {error|=ERROR_PARAM;}

  /* Copy and perform math */
  if(!error){
    state->colStart = cfg->colStart;
    state->rowStart = cfg->rowStart;
    state->numCol = cfg->numCol;
    state->numRow = cfg->numRow;
    /* Derived params */
    state->rowEnd = state->rowStart + state->numRow - 1;
    state->colEnd = state->colStart + state->numCol - 1;
    state->pageStart = (state->rowStart / SSD1306_PAGE_HEIGHT);
    state->pageEnd = (state->rowEnd / SSD1306_PAGE_HEIGHT);
    state->numPage = 1 + (state->pageEnd - state->pageStart);
    /* Validate derived */
    if(state->rowEnd >= SSD1306_NUM_ROWS){error|=ERROR_VAL;}
    if(state->colEnd >= SSD1306_NUM_COLS){error|=ERROR_VAL;}
    if(state->pageEnd >= SSD1306_NUM_PAGE-1){error|=ERROR_VAL;} /* Can't be at bottom page*/
    /* Initialize the vertical axis */
    display_line_cfg_s verticalAxisCfg = display_line_cfg_default;
    verticalAxisCfg.colStart = state->colStart-1;
    verticalAxisCfg.numCol = 1;
    verticalAxisCfg.rowStart = state->rowStart;
    verticalAxisCfg.numRow = state->numRow+2;
    display_line_init(&state->axis_vertical, &verticalAxisCfg);
    /* Initialize the horizontal axis */
    display_line_cfg_s horizontalAxisCfg = display_line_cfg_default;
    horizontalAxisCfg.colStart = state->colStart-2;
    horizontalAxisCfg.numCol = state->numCol+3;
    horizontalAxisCfg.rowStart = state->rowEnd+1;
    horizontalAxisCfg.numRow = 1;
    display_line_init(&state->axis_horizontal, &horizontalAxisCfg);
    /* Mark as successful*/
    if(!error){
      state->_isInit = true;
      state->shouldGraphRender = true;
    }
  }
  if(error){state->_isInit=false;}
  return error;
}

const display_graph_cfg_s display_graph_cfg_default = {
  .rowStart=0,
  .colStart=0,
  .numCol=0,
  .numRow=0,
};

const display_line_cfg_s display_line_cfg_default = {
  .rowStart=0,
  .colStart=0,
  .numCol=0,
  .numRow=0,
};

/*******************************************************************************
* Function Name: display_updateGraphColumn()
********************************************************************************
* \brief
*   Update a specific column of the graph
*
* \return
*  None
*******************************************************************************/
uint32_t display_updateGraphColumn(ssd1306_state_s *const state, display_graph_s *const graph, uint8_t colIdx, uint8_t val){
  uint32_t error = 0;
  if(!state->_isInitialized){error|=ERROR_INIT;}
  if(!graph->_isInit){error|=ERROR_INIT;}
  if(colIdx>graph->numCol){error|=ERROR_VAL;}
  
  if(!error){
    /* Set the window - Only writing one column so start and end match */
    display_window_s window;
    window.pageStart = graph->pageStart;
    window.pageEnd = graph->pageEnd;
    window.colStart  = graph->colStart + colIdx;
    window.colEnd = window.colStart;
    error |= SSD1306_setWindow(state, &window);
    if(!error){
      /* Saturate at top of graph */
      if(val>(graph->numRow-1)){val=(graph->numRow-1);}
      /* Map the data onto the column */
      uint64_t colData;
      colData = (uint64_t) ((uint64_t) 1 << val);
      /* Convert uint64_t to column */
      uint8_t colDataBuffer[graph->numPage];
      for(uint8_t pageIdx =0; pageIdx<graph->numPage; pageIdx++){
        colDataBuffer[graph->numPage-pageIdx-1] = reverseBits((uint8_t) (colData >> (BITS_PER_BYTE *pageIdx)));
      }
      error |= SSD1306_writeDataArray(state, colDataBuffer, graph->numPage);   
    }
  }
  return error;
}

/*******************************************************************************
* Function Name: display_updateGraphScroll()
********************************************************************************
* \brief
*   Update all of the graph 
*
* \return
*  None
*******************************************************************************/
uint32_t display_updateGraphScroll(ssd1306_state_s *const state, display_graph_s *const graph, mjl_ring_s *const ring){
  uint32_t error = 0;
  if(!state->_isInitialized){error|=ERROR_INIT;}
  if(!ring->_init){error|=ERROR_INIT;}
  if(!graph->_isInit){error|=ERROR_INIT;}
  
  if(!error){
    for(uint8_t i=0; i<ring->count; i++){
      uint8_t val = ring->buffer[(ring->tail+i)%ring->size];
      error |= display_updateGraphColumn(state, graph, i, val);
      if(error){break;}
    }
  }
  return error;
}


/*******************************************************************************
* Function Name: reverseBits()
********************************************************************************
* \brief
*   Reverse the bit order of a byte 
*
* \return
*  None
*******************************************************************************/
uint8_t reverseBits(uint8_t byte) {
  uint8_t reversedByte = 0;
  for(uint8_t i=0; i<BITS_PER_BYTE; i++) {
    if((byte & (1 << i)) != 0){
      reversedByte |= (1 << ((BITS_PER_BYTE-1) - i));
    }
  }
  return reversedByte;
}

/* [] END OF FILE */