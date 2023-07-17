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
#include "mjl_ringBuffer.h"

/* Default config struct */
const mjl_ring_cfg_s mjl_ring_cfg_default = {
  .buffer = NULL,
  .size = 0,
  .overWrite = false,
};

/*******************************************************************************
* Function Name: mjl_ringBuffer_init()
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
uint32_t mjl_ringBuffer_init(mjl_ring_s *const state, mjl_ring_cfg_s *const cfg){
  uint32_t error = 0;
  /* Verify required functions */
  error |= (NULL == state) ? ERROR_POINTER : ERROR_NONE;
  error |= (NULL == cfg) ? ERROR_POINTER : ERROR_NONE;
  error |= (NULL == cfg->buffer) ? ERROR_POINTER : ERROR_NONE;
  error |= (0 == cfg->size) ? ERROR_VAL: ERROR_NONE;
  /* external start is not required */
  /* external stop is not required */
  /* Valid Inputs */
  if(!error) {
    /* Copy params */
    state->buffer = cfg->buffer;
    state->size = cfg->size;
    state->overWrite = cfg->overWrite;
    /* Set default vals */
    state->head = 0;
    state->tail = 0;
    state->count = 0;
    /* Mark as initialized */
    state->_init = true;
  }
  if(error){state->_init=false;}
  return error;
}

/*******************************************************************************
* Function Name: mjl_ringBuffer_enqueue()
********************************************************************************
* \brief
*   Add an element to the queue
*
* \param state [in/out]
* Pointer to the state struct
*
* \param in [in]
* Value to add to the queue 
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t mjl_ringBuffer_enqueue(mjl_ring_s *const state, uint32_t in){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}

  if(!error){
    if(mjl_ringBuffer_isBufferFull(state)){
      if(false == state->overWrite){error|=ERROR_STATE;}
      /* Buffer is full, but overwrite*/
      else {
        state->buffer[state->head] = in;
        state->head = (state->head + 1) % state->size;
        state->tail = (state->tail + 1) % state->size;
      }
    }
    /* Buffer is not full, continue filling */
    else{
      state->buffer[state->head] = in;
      state->head = (state->head + 1) % state->size;
      state->count++;
    }

  }
  return error;
}


/*******************************************************************************
* Function Name: mjl_ringBuffer_dequeue()
********************************************************************************
* \brief
*   Remove an element from the queue 
*
* \param state [in/out]
* Pointer to the state struct
*
* \param out [out]
*  Value removed from the queue
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t mjl_ringBuffer_dequeue(mjl_ring_s *const state, uint32_t *out){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(mjl_ringBuffer_isBufferEmpty(state)){error|=ERROR_STATE;}

  if(!error){
    *out = state->buffer[state->tail];
    state->tail = (state->tail+1) % state->size;
    state->count--;
  }
  return error;
}

/*******************************************************************************
* Function Name: mjl_ringBuffer_isBufferFull()
********************************************************************************
* \brief
*   Check to see if the buffer is full 
*
* \param state [in/out]
* Pointer to the state struct
*
* \return
*  Result of the operation
*******************************************************************************/
inline bool mjl_ringBuffer_isBufferFull(mjl_ring_s *const state){
  return state->count == state->size;
}

/*******************************************************************************
* Function Name: mjl_ringBuffer_isBufferEmpty()
********************************************************************************
* \brief
*   Check to see if the buffer is empty 
*
* \param state [in/out]
* Pointer to the state struct
*
* \return
*  Result of the operation
*******************************************************************************/
inline bool mjl_ringBuffer_isBufferEmpty(mjl_ring_s *const state){
  return state->count == 0;
}
/* [] END OF FILE */