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

/*******************************************************************************
* Function Name: mjl_calculateForce()
********************************************************************************
* \brief
*   Calculate the force given the ADC word, gain val, dacword
*
* \param force [in/out]
*   Pointer to the force struct
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t mjl_calculateForce(mjl_force_s *const state){
  uint32_t error = 0; 
  /* [µV] = [µV/nV] x [nA/count] x [counts] x [Ω] */
  uint64_t v_ref_uv  = 1E3 * state->k_dac_na_per_count * state->dacWord * state->r_scale_ohm;
  /* [µV] = [µV/count] x [count] */
  uint64_t v_adc_uv = state->k_adc_uv_per_count * state->adcWord;
  /* Ensure bounds */
  if(v_ref_uv > v_adc_uv){error|=ERROR_VAL;}
  else{
    /* [µV] = ([µV] - [µV]) / [V/V] */
    uint64_t v_strain_uv = (v_adc_uv - v_ref_uv); 
    /* [gf] = [µV] x [gf/µV] */
    uint64_t force_gf =  v_strain_uv * (state->k_force_gf_per_uv) / ((uint64_t) state->gainVal);
    state->force_gf_out = (uint32_t) force_gf;
  }
  return error;
}

/* [] END OF FILE */