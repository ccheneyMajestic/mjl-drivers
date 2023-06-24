/***************************************************************************
*                                Majestic Labs © 2023
* File: mjl_force.c
* Workspace: MJL Drivers
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: Calculating force from a strain gauge
*
* 2023.06.22 - Document Created
********************************************************************************/
#include "mjl_force.h"
#include "mjl_errors.h"

const mjl_force_cfg_s mjl_force_cfg_default = {
  .k_force_gf_per_v = 0,
  .k_adc_uv_per_count = 0,
  .k_dac_na_per_count = 0,
  .r_scale_ohm = 0,
};

/*******************************************************************************
* Function Name: mjl_force_init()
********************************************************************************
* \brief
*   Initialize the state structure
*
* \param state [in/out]
*   Pointer to the state struct
*
* \param cfg [in]
*   Pointer to the configuration struct
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t mjl_force_init(mjl_force_s *const state, mjl_force_cfg_s *const cfg){
  uint32_t error = 0;
  /* Verify required functions */
  error |= (NULL == state) ? ERROR_POINTER : ERROR_NONE;
  error |= (NULL == cfg) ? ERROR_POINTER : ERROR_NONE;
  error |= (0 == cfg->k_force_gf_per_v) ? ERROR_VAL : ERROR_NONE;
  error |= (0 == cfg->k_adc_uv_per_count) ? ERROR_VAL : ERROR_NONE;
  error |= (0 == cfg->k_dac_na_per_count) ? ERROR_VAL : ERROR_NONE;
  /* Valid Inputs */
  if(!error) {
    /* Copy params */
    state->k_force_gf_per_v = cfg->k_force_gf_per_v;
    state->k_adc_uv_per_count = cfg->k_adc_uv_per_count;
    state->k_dac_na_per_count = cfg->k_dac_na_per_count;
    state->r_scale_ohm = cfg->r_scale_ohm;
    /* Mark as initialized */
    state->_init = true;
    /* Set initial params */
    state->adcWord = 0;
    state->gainVal = 0;
    state->dacWord = 0;
    state->out_force_gf = 0;
    state->offset_gf = 0;
  }
  if(error){state->_init=false;}
  return error;
}

/*******************************************************************************
* Function Name: mjl_force_update()
********************************************************************************
* \brief
*   Calculate the force given the ADC word, gain val, dacword
*
* \param state [in/out]
*   Pointer to the state struct
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t mjl_force_update(mjl_force_s *const state){
  uint32_t error = 0; 
  if(!state->_init){error|=ERROR_INIT;}
  if(0 == state->gainVal){error|=ERROR_PARAM;}

  if(!error){
    /* [µV] = [µV/nV] x [nA/count] x [counts] x [Ω] */
    uint64_t v_ref_uv  = 1000 * state->k_dac_na_per_count * state->dacWord * state->r_scale_ohm;
    /* [µV] = [µV/count] x [count] */
    uint64_t v_adc_uv = state->k_adc_uv_per_count * state->adcWord;
    /* Ensure bounds */
    if(v_ref_uv > v_adc_uv){error|=ERROR_VAL;}
    else{
      if(0 == state->gainVal){state->out_force_gf = 0;}
      /* [µV] = ([µV] - [µV]) */
      uint64_t v_strain_uv = (v_adc_uv - v_ref_uv); 
      /* [µgf] = [µV] x [gf/V]  */
      uint64_t force_ugf =  v_strain_uv * (state->k_force_gf_per_v); 
      /* [gf] = [µgf] / [µV/V] */
      uint64_t force_gf = force_ugf / 1000000;
      /* Scale down - [gf] = [gf] / [V/V] */
      force_gf /= ((uint64_t) state->gainVal);
      /* Subtract offset */
      if(force_gf > state->offset_gf){
        force_gf -= state->offset_gf;
        state->out_force_gf = (uint32_t) force_gf;
      }
      else{error |= ERROR_VAL;}
      /* Save to state */
    }
  }
  if(error){state->out_force_gf = 0;}

  return error;
}

/* [] END OF FILE */