
/***************************************************************************
*                                Majestic Labs © 2023
* File: hal_psoc4.c
* Workspace: MJL Hardware Abstraction Layer (HAL) Library 
* Version: v1.0.0
* Author: C. Cheney
* Target: PSoC4 CYBLE_222014-01 Module
*
* Brief: Serial communication middleware for the MJL driver library 
*
* 2023.04.26  - Document Created
********************************************************************************/
#include "hal_psoc4.h"
#include "mjl_errors.h"
#include "project.h"    /* Cypress project files*/

/*******************************************************************************
* Function Name: uart_psoc4SCB_start()
********************************************************************************
* \brief
*   Start wrapper for a SCB based UART on PSoC4
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t uart_psoc4SCB_start(MLJ_UART_T *const state){   
    uint32_t error = 0;   
    (void) state;
    uartUsb_Start();
    return error;
}

/*******************************************************************************
* Function Name: uart_psoc4SCB_stop()
********************************************************************************
* \brief
*   Stop wrapper for a SCB based UART on PSoC4
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t uart_psoc4SCB_stop(MLJ_UART_T *const state){   
    uint32_t error = 0;   
    (void) state;
    uartUsb_Stop();
    return error;
}

/*******************************************************************************
* Function Name: uart_psoc4SCB_writeArrayBlocking()
********************************************************************************
* \brief
*   Wrapper for an SCB Based UART on PSoC4
*   Write an array of data via UART using the SCB api. This is a blocking function
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t uart_psoc4SCB_writeArrayBlocking(const uint8_t *array, uint16_t len){
    uint32_t error = 0;   
    uartUsb_SpiUartPutArray(array, len);
    return error;
}

/*******************************************************************************
* Function Name: uart_psoc4SCB_read()
********************************************************************************
* \brief
*   Wrapper for an SCB Based UART on PSoC4
*   Write an array of data via UART using the SCB api. This is a blocking function
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t uart_psoc4SCB_read(uint8_t *data){
    uint32_t error = 0;   
    if(0 == uartUsb_SpiUartGetRxBufferSize()){error|= ERROR_UNAVAILABLE;}
    else {*data = uartUsb_SpiUartReadRxData();}
    return error;
}


/* [] END OF FILE */