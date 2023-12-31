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
/* Header Guard */
#ifndef TFT_GC9107_H
  #define TFT_GC9107_H
  /***************************************
  * Included files
  ***************************************/
  #include <stdbool.h>
  #include <mjl_errors.h>
  /***************************************
  * Macro Definitions
  ***************************************/
  #define LCD_PIXEL_HORZ  (128)
  #define LCD_PIXEL_VERT  (115)


  #define LCD_DELAY_RESET     (150) /* Time in [ms] for the reset supervisor */
  #define LCD_DELAY_WAKEUP    (5) /* Time in [ms] to wakeup */
  #define LCD_DC_DATA         (1) /* Data mode for the LCD Screen */
  #define LCD_DC_COMMAND      (0) /* Command mode for the LCD Screen */
  
  #define LCD_CMD_SLEEPOUT    (0x11) /* Turn off sleep mode */
  #define LCD_CMD_INVERT_OFF  (0x20) /* Display Inversion Off - Normal display */
  #define LCD_CMD_INVERT_ON   (0x21) /* Display Inversion On - Invert the display */
  #define LCD_CMD_DISPOFF     (0x28) /* Display off - turn the display off */
  #define LCD_CMD_DISPON      (0x29) /* Display on - turn the display on */
  #define LCD_CMD_COLSET      (0x2A) /* Column Address Set */
  #define LCD_CMD_ROWSET      (0x2B) /* Row Address Set */
  #define LCD_CMD_MEMWRITE    (0x2C) /* Memory write to frame memory */
  #define LCD_CMD_COLMOD      (0x3A) /* Pixel format - depth of data */
  
  #define LCD_COLMOD_BITS_16  (0b101) /* 16 bits per pixel */
  
  #define COLOR_MASK_R (1 << 0)
  #define COLOR_MASK_G (1 << 1)
  #define COLOR_MASK_B (1 << 2)
  
  /***************************************
  * Enumerated types
  ***************************************/
  /* B | G | R */
  typedef enum {
    BLACK = 0b000,
    RED   = 0b001,
    GREEN = 0b010,
    YELLOW  = 0b011,
    BLUE  = 0b100,
    MAGENTA  = 0b101,
    CYAN  = 0b110,
    WHITE = 0b111,
  } RGB_COLORS_T;

  /***************************************
  * Structures 
  ***************************************/
  
  typedef struct {
    uint16_t x;
    uint16_t y;
  } GR_POINT_S;
  
  typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
  } rgb_s;
  
 /* Window for the struct */
  typedef struct {
    GR_POINT_S start;
    GR_POINT_S end;
    uint16_t numCols;
    uint16_t numRows;
  } LCD_WINDOW_S;

  // /* Configuration Structure */
  // typedef struct {
  //   uint32_t (*hal_req_function)(uint32_t data);  /* Required HAL function */
  //   uint32_t (*hal_opt_start)(MLJ_TEMPLATE_T *const state);  /* Optional HAL function */
  //   uint32_t (*hal_opt_stop)(MLJ_TEMPLATE_T *const state);  /* Optional HAL function */
  // } MJL_TEMPLATE_CFG_S;

  // /* Serial State Object   */
  // typedef struct {
  //   uint32_t (*hal_req_function)(uint32_t data);  /* Required HAL function */
  //   uint32_t (*hal_opt_start)(MLJ_TEMPLATE_T *const state);  /* Optional HAL function */
  //   uint32_t (*hal_opt_stop)(MLJ_TEMPLATE_T *const state);  /* Optional HAL function */

  //   bool _init;
  //   bool _running;
  //   bool isLoggingEnabled;
  // } MLJ_TEMPLATE_S;

  /* Default config struct */
  // extern const MJL_TEMPLATE_CFG_S template_cfg_default;
  extern LCD_WINDOW_S window;
  extern rgb_s color;
  /***************************************
  * Function declarations 
  ***************************************/
  uint32_t lcd_init(void);
  uint32_t lcd_wakeup(void);
  uint32_t lcd_writeCommand(uint8_t cmd);
  uint32_t lcd_writeCommand_withParam(uint8_t cmd, const uint8_t * param, uint16_t len);
  uint32_t lcd_writeData(uint8_t * data, uint16_t len);
  uint32_t lcd_setDisplayMode(bool state);
  uint32_t lcd_setWindow(const LCD_WINDOW_S * window);
  uint32_t lcd_writeRgb16_window(LCD_WINDOW_S * window, rgb_s * row);
  uint32_t lcd_drawColor(LCD_WINDOW_S *window, rgb_s * color);
  uint32_t colorName2Rgb(RGB_COLORS_T color, rgb_s * rgb, uint8_t intensity);

  
  void uint16_to_uint8(const uint16_t * in, uint8_t * out, uint16_t len);
  /* State Operations */
  // uint32_t template_init(MLJ_TEMPLATE_S *const state, MJL_TEMPLATE_CFG_S *const cfg);
  // uint32_t template_start(MLJ_TEMPLATE_S *const state);
  // uint32_t template_stop(MLJ_TEMPLATE_S *const state);

#endif /* TFT_GC9107_H */
/* [] END OF FILE */
