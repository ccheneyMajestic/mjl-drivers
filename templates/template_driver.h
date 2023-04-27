/***************************************************************************
*                                Majestic Labs © 2023
* File: template_driver.h
* Workspace: MJL Driver Library 
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: Read-only template for a Majestic Labs Driver  
*
* 2023.04.27  - Document Created
********************************************************************************/
/* Header Guard */
#ifndef TEMPLATE_H
  #define TEMPLATE_H
  /***************************************
  * Included files
  ***************************************/
  #include <stdbool.h>
  #include <mjl_errors.h>
  /***************************************
  * Macro Definitions
  ***************************************/

  /***************************************
  * Enumerated types
  ***************************************/

  /***************************************
  * Structures 
  ***************************************/
  /* Forward declare struct */
  typedef struct MLJ_TEMPLATE_S MLJ_TEMPLATE_T;
  /* Configuration Structure */
  typedef struct {
    uint32_t (*hal_req_function)(uint32_t data);  /* Required HAL function */
    uint32_t (*hal_opt_start)(MLJ_TEMPLATE_T *const state);  /* Optional HAL function */
    uint32_t (*hal_opt_stop)(MLJ_TEMPLATE_T *const state);  /* Optional HAL function */
  } MJL_TEMPLATE_CFG_S;

  /* Serial State Object   */
  typedef struct {
    uint32_t (*hal_req_function)(uint32_t data);  /* Required HAL function */
    uint32_t (*hal_opt_start)(MLJ_TEMPLATE_T *const state);  /* Optional HAL function */
    uint32_t (*hal_opt_stop)(MLJ_TEMPLATE_T *const state);  /* Optional HAL function */

    bool _init;
    bool _running;
    bool isLoggingEnabled;
  } MLJ_TEMPLATE_S;

  /* Default config struct */
  extern const MJL_TEMPLATE_CFG_S template_cfg_default;
  /***************************************
  * Function declarations 
  ***************************************/
  /* State Operations */
  uint32_t template_init(MLJ_TEMPLATE_S *const state, MJL_TEMPLATE_CFG_S *const cfg);
  uint32_t template_start(MLJ_TEMPLATE_S *const state);
  uint32_t template_stop(MLJ_TEMPLATE_S *const state);

#endif /* TEMPLATE_H */
/* [] END OF FILE */
