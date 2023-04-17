/***************************************************************************
*                             Majestic Labs © 2023
*
* File: mjl_errors.h
* Workspace: MJL Driver Library 
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: Common errors for the MJL library 
*   
*
* 2023.04.17  - Document Created
********************************************************************************/

/* Header Guard */
#ifndef MJL_ERRORS_H
  #define MJL_ERRORS_H
  /***************************************
  * Included files
  ***************************************/
  #include <stdint.h>
  #include <stdbool.h>
  /***************************************
  * Macro Definitions
  ***************************************/
  #define ERROR_SHIFT_POINTER       (0) /* Shift of a null pointer */
  #define ERROR_SHIFT_INIT          (1) /* Shift of an uninitialized structure */
  #define ERROR_SHIFT_VAL           (2) /* Shift of a bad value error */

  #define ERROR_NONE                (0)   /* No error */
  #define ERROR_POINTER             (1 << ERROR_SHIFT_POINTER) 
  #define ERROR_INIT                (1 << ERROR_SHIFT_INIT) 
  #define ERROR_VAL                 (1 << ERROR_SHIFT_VAL) 
  /***************************************
  * Enumerated Types
  ***************************************/

  /***************************************
  * Structures
  ***************************************/

  /***************************************
  * External variables
  ***************************************/

  /***************************************
  * Function declarations 
  ***************************************/



#endif /* MJL_ERRORS_H*/
/* [] END OF FILE */