/***************************************************************************
*                             Majestic Labs © 2022
*
* File: DRV8244.h
* Workspace: mjl_drivers
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: Driver functions for the DRV8244 Full bridge driver 
*   
*
* 2022.04.02  - Document Created
********************************************************************************/

/* Header Guard */
#ifndef DRV8244_H
  #define DRV8244_H
  /***************************************
  * Included files
  ***************************************/
  #include <stdint.h>
  #include <stdbool.h>
  /***************************************
  * Macro Definitions
  ***************************************/
  #define DRV8244_VAL_AWAKE             (1) /* Write to the nSLEEP pin on the DRV8244 to wake up */
  #define DRV8244_VAL_ASLEEP            (0) /* Write to the nSLEEP pin on the DRV8244 to go to sleep*/
  #define DRV8244_VAL_DISABLED           (1) /* Write to the DRVOFF pin on the DRV8244 to disable */
  #define DRV8244_VAL_ENABLED            (0) /* Write to the DRVOFF pin on the DRV8244 to go to enabled*/
  #define DRV8244_VAL_FAULT             (0)  /* read  val on nFAULT pin if there is an error */
  #define DRV8244_PERIOD_RESET          (30) /* Time in [µs] to clear the fault */
  #define DRV8244_PERIOD_SLEEP          (80) /* Time in [µs] to sleep the device */
  #define DRV8244_PERIOD_WAKE          (1000) /* Time in [µs] to sleep the device */

  
  
  #define DRV84244_ERROR_FAULT          (1u << 0) /* A fault is present on the nFault pin */
  #define DRV84244_ERROR_INIT           (1u << 1) /* Failed to initialize the device */
  #define DRV84244_ERROR_SYNC           (1u << 2) /* Hardware is not synchronized */
  #define DRV84244_ERROR_LOCKED         (1u << 3) /* Hardware is not synchronized */




  /***************************************
  * Enumerated Types
  ***************************************/
  typedef enum{
    DRV8244_STATE_OFF, 
    DRV8244_STATE_AWAKE,
    DRV8244_STATE_ENABLED,

  } drv8244_state_t;
  /***************************************
  * Structures
  ***************************************/
  /* Configuration Structure */
  typedef struct {
    /* Function pointers */


  } drv8244_config_s;


  
  /* State Structure */
  typedef struct {
    bool _isObjectInitialized;
    bool _isObjectLocked;
    bool isRunning;

    /* State Variables */
    drv8244_state_t state_previous;
    drv8244_state_t state_current;
    drv8244_state_t state_next;
    bool isEnabled;


    /* Outputs */
    uint32_t status;
    uint32_t output;
    /* State variables */
    uint32_t time_locked; 
    uint32_t time_running; 
    uint32_t time_lastUpdate; 


  } drv8244_state_s;
  
  /***************************************
  * Function declarations 
  ***************************************/
  uint32_t drv8244_init(drv8244_state_s *const state, drv8244_config_s *const cfg);
  uint32_t drv8244_start(drv8244_state_s *const state);
  uint32_t drv8244_stop(drv8244_state_s *const state);
  uint32_t drv8244_enable(drv8244_state_s *const state);
  uint32_t drv8244_disable(drv8244_state_s *const state);


#endif /* DRV8244_H*/
/* [] END OF FILE */