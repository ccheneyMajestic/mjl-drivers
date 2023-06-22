/***************************************************************************
*                                Majestic Labs © 2023
* File: mjl_force.h
* Workspace: MJL Drivers
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: Calculating force from a strain gauge
*
* 2023.06.22 - Document Created
********************************************************************************/
/* Header Guard */
#ifndef MJL_FORCE_H
  #define MJL_FORCE_H
  /***************************************
  * Included files
  ***************************************/
  #include <stdint.h>
  /***************************************
  * Macro Definitions
  ***************************************/

  
  /***************************************
  * Enumerated types
  ***************************************/
  typedef enum{
    MJL_UNITS_KG,
    MJL_UNITS_LBS,
  }mjl_force_units_s;
  /***************************************
  * Structures 
  ***************************************/
  /* Exercise structure */
  typedef struct{
    uint16_t adcWord;     /* ADC [counts] */
    uint16_t gainVal;     /* Gain [V/V] */
    uint8_t dacWord;      /* DAC [counts] */
  } sl_exercise_s;
  /* Force Constants */
  typedef struct{
    uint16_t adcWord;               /* ADC [counts] */
    uint16_t gainVal;               /* Gain [V/V] */
    uint8_t dacWord;                /* DAC [counts] */
    uint32_t k_force_gf_per_uv;     /* Force constant [gf/µV] */
    uint32_t k_adc_uv_per_count;    /* ADC constant [nV/count] */
    uint32_t k_dac_na_per_count;    /* DAC constant [nA/count] */
    uint32_t r_scale_ohm;           /* DAC resistor [Ω] */
    uint32_t force_gf_out;          /* Calculated force [gf] */
    mjl_force_units_s units;        /* Output units*/
  } mjl_force_s;
  
  /***************************************
  * Function declarations 
  ***************************************/
  uint32_t mjl_calculateForce(mjl_force_s *const state);

  

#endif /* MJL_FORCE_H */
/* [] END OF FILE */
