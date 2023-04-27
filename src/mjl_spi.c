/***************************************************************************
*                                Majestic Labs © 2023
* File: mjl_spi.h
* Workspace: MJL Driver Library 
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: Serial Peripheral Interface middleware driver 
*   
*
* 2023.04.26  - Document Created
********************************************************************************/
#include "mjl_spi.h"

const MJL_SPI_CFG_S spi_cfg_default = {
  req_fn_writeArray
  req_fn_read
  opt_fn_externalStart
  opt_fn_externalStop
};


/* [] END OF FILE */
