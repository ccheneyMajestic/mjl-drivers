/***************************************************************************
*                                Majestic Labs © 2023
* File: FLASH_IS25LP.h
* Workspace: MJL Driver Library 
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: Driver file for the IS25LP Flash module from ISSI
*
* 2023.05.25 - Document Created 
********************************************************************************/
#include "FLASH_IS25LP.h"

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
//     state->isLoggingEnabled = true;
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