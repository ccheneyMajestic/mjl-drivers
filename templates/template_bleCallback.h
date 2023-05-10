/***************************************************************************
*                                Majestic Labs © 2023
* File: SL_ble.h
* Workspace: Strength Loop v4
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: Bluetooth application for Strength Loop 
*
* 2023.05.03  - Document Created
********************************************************************************/
/* Header Guard */
#ifndef SL_BLE_H
  #define SL_BLE_H
  /***************************************
  * Included files
  ***************************************/
  #include <stdbool.h>
  #include <mjl_errors.h>
  #include "project.h"
  
  /***************************************
  * Macro Definitions
  ***************************************/

  /***************************************
  * Enumerated types
  ***************************************/

  /***************************************
  * Structures 
  ***************************************/
  typedef struct {
    CYBLE_CONN_HANDLE_T connHandle; /* Connection Handle */
  } SL_BLE_S;

  /***************************************
  * Function declarations 
  ***************************************/
  /* State Operations */
  void strengthLoop_ble(uint32_t eventCode, void * eventParam);


#endif /* SL_BLE_H */
/* [] END OF FILE */
