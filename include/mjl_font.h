/***************************************************************************
*                             Majestic Labs © 2023
*
* File: mjl_font.h
* Workspace: MJL Driver Library 
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: The majestic lab font
*   
*
* 2023.04.17  - Document Created
********************************************************************************/

/* Header Guard */
#ifndef MJL_FONT_H
  #define MJL_FONT_H
  /***************************************
  * Included files
  ***************************************/
  #include <stdint.h>
  #include <stdbool.h>
  /***************************************
  * Macro Definitions
  ***************************************/
  #define UI_TEXT_8x16_LEN                    (16) /* Number of byte in an 8x16 character */
  #define UI_ICON_BATTERY_LEN                 (16) /* number of bytes in the battery icon */
  #define UI_ICON_HISTORY_LEN                 (48) /* number of bytes in the history icon */
  #define UI_ICON_RESET_LEN                   (48) /* number of bytes in the reset icon */
  /***************************************
  * External Vars
  ***************************************/
  extern const uint8_t* digits_8x16[10];
  extern const uint8_t* alphabet_8x16[26];
  extern const uint8_t battery_1bar[UI_ICON_BATTERY_LEN];
  extern const uint8_t historyOutline[UI_ICON_HISTORY_LEN];
  extern const uint8_t resetOutline[UI_ICON_RESET_LEN];

  /***************************************
  * Structures
  ***************************************/

  /***************************************
  * External variables
  ***************************************/

  /***************************************
  * Function declarations 
  ***************************************/



#endif /* MJL_FONT_H*/
/* [] END OF FILE */