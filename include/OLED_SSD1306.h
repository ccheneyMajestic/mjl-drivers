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

/* Header Guard */
#ifndef SSD1306_H
  #define SSD1306_H
  /***************************************
  * Included files
  ***************************************/
  #include <stdint.h>
  #include <stdbool.h>
  /***************************************
  * Macro Definitions
  ***************************************/

  /***************************************
  * Enumerated Types
  ***************************************/
  /* Enumerated State Types */
  typedef enum {
    SSD1306_STATE_OFF,
    SSD1306_STATE_ON,
  } ssd1306_state_t;

  /* Transition causes */
  typedef enum {
      SSD1306_TRANSITION_NONE,
      SSD1306_TRANSITION_CMD,
  } ssd1306_state_transition_t;
  
  /***************************************
  * Structures
  ***************************************/
  /* Object Configuration Structure */
  typedef struct{
    /* HAL Function Pointers */
    uint32_t (*fn_getTime_ms) (void);
    /* Object function pointer */
  } ssd1306_cfg_s;

  /* Object State Structure */
  typedef struct {
    /* Required Variables */
    bool _isInitialized;
    bool _isLocked;
    uint32_t output;
    uint32_t error;
    /* Input parameters */
    uint32_t (*fn_getTime_ms) (void);
    /* Nested objects */

    /* State variables */
    uint32_t stateCount;
    ssd1306_state_t state_current;
    ssd1306_state_t state_next;
    ssd1306_state_t state_previous;
    ssd1306_state_t state_requested;
    /* Flags and status */
    bool flag_isStateTransitionRequested;
    bool flag_didStateTransitionOccur;
    ssd1306_state_transition_t stateTransitionCause;
    uint32_t status_timeLastUpdated; 
    /* Actuator Values */

    /* Sensor values */

  } ssd1306_state_s;
  
  /***************************************
  * Function declarations 
  ***************************************/
  /* Required Functions */
  uint32_t ssd1306_start(ssd1306_state_s *const state, ssd1306_cfg_s *const cfg);
  uint32_t ssd1306_stop(ssd1306_state_s *const state);

  /* Actions */


#endif /* SSD1306_H*/
/* [] END OF FILE */