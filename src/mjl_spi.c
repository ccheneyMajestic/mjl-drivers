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
  .req_hal_writeArray=NULL,
  .req_hal_read=NULL,
  .opt_hal_externalStart=NULL,
  .opt_hal_externalStop=NULL,
};


/* [] END OF FILE */
