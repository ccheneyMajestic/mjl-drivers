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

/* Default config struct */
const FLASH_IS25_CFG_S flash_is25_cfg_default = {
  .fn_delayUs = NULL,
  .fn_spi_writeArrayBlocking = NULL,
  .fn_spi_readArrayBlocking = NULL,
  .slaveId = 0,
};


/*******************************************************************************
* Function Name: flash_is25_init()
********************************************************************************
* \brief
*   Initializes the state struct from a configuration struct 
*
* \param state [in/out]
* Pointer to the state struct
* 
* \param cfg [in]
* Pointer to the configuration struct
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t flash_is25_init(FLASH_IS25_S *const state, FLASH_IS25_CFG_S *const cfg){
  uint32_t error = 0;
  /* Verify required functions */
  error |= (NULL == cfg->fn_delayUs) ? ERROR_POINTER : ERROR_NONE;
  error |= (NULL == cfg->fn_spi_writeArrayBlocking) ? ERROR_POINTER : ERROR_NONE;
  error |= (NULL == cfg->fn_spi_readArrayBlocking) ? ERROR_POINTER : ERROR_NONE;
  /* external start is not required */
  /* external stop is not required */
  /* Valid Inputs */
  if(!error) {
    /* Copy params */
    state->fn_delayUs = cfg->fn_delayUs;
    state->fn_spi_writeArrayBlocking = cfg->fn_spi_writeArrayBlocking;
    state->fn_spi_readArrayBlocking = cfg->fn_spi_readArrayBlocking;
    state->slaveId = cfg->slaveId;
    /* Mark as initialized */
    state->_init = true;
    state->_running = false;
  }
  if(error){state->_init=false;}
  return error;
}

/*******************************************************************************
* Function Name: flash_is25_start()
********************************************************************************
* \brief
*   Starts the Object and call the external start function
*   Checks the Manufacturer ID to ensure the device is started properly
*
* \param state [in/out]
* Pointer to the state struct
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t flash_is25_start(FLASH_IS25_S *const state){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(state->_running){error|=ERROR_RUNNING;}

  if(!error){
    uint8_t cmd = 0;
    /* Reset the device*/
    cmd = IS25_CMD_RSTREN; /* Reset enable */
    error|=state->fn_spi_writeArrayBlocking(state->slaveId, &cmd, 1);
    cmd = IS25_CMD_RST; /* Reset */
    error|=state->fn_spi_writeArrayBlocking(state->slaveId, &cmd, 1);
    state->fn_delayUs(IS25_US_RESET);
    /* Read from the Manufacturer ID */
    uint8_t idArray[IS25_RDMDID_LEN] = {0x00};
    idArray[0] = IS25_CMD_RDMDID;
    error|=state->fn_spi_readArrayBlocking(state->slaveId, idArray, IS25_RDMDID_LEN);
    bool doesMfgIdMatch = (idArray[IS25_RDMDID_POS_MFG] == IS25_ID_MFG);
    bool doesDevIdMatch = (idArray[IS25_RDMDID_POS_DEV] == IS25_ID_DEV);
    if(doesMfgIdMatch && doesDevIdMatch){state->_running = true;}
    else{error|=ERROR_VAL;}
  }
  if(error){state->_running=false;}
  return error;
}

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