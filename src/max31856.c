/***************************************************************************
*                              MICA  © 2019
*                               
*
* File: max31856.c
* Workspace: cloudChamber_v5.1
* Project: Thermocouple_v5.1
* Version: 1.0.0
* Authors: C. Cheney
* 
* PCB: mcuPSoC4 5.2.1
* mcuType: PSoC
* partNumber:CY8C4245LQI-483
*
* Brief:
*   Controling the MAX31856 thermocouple frontend
*
* 2019.06.20  - Document Created
********************************************************************************/
#include "max31856.h"
#include "micaCommon.h"


/*******************************************************************************
* Function Name: MAX31856_readReg()
****************************************************************************//**
* \brief
*   Reads a register from the MAX31856 
*
* \param state [in/out]
*   Pointer to the state struct
*
* \param addr [in]
*   Address of the register
*
* \param val [out]
*   Return value of the register 
*  
*
* \return
*   Error code of the operation
*******************************************************************************/
uint32_t MAX31856_readReg(MAX31856_STATE_S* state, uint8_t addr, uint8_t *val){
    /* Ensure state exists */
    uint32_t error = Comms_validateSpi(state->spi);
    if(!error) {
        /* Set slave active */
        error|= state->spi->setActive(state->slaveId);
        /* Places results into val */
        uint32_t spiError = state->spi->read(addr, val);
        /* Place output error in the state struct */
        error |= spiError ? COMMS_ERROR_SPI : COMMS_ERROR_NONE;
        if(error){state->error = spiError;}
    }
    return error;
}

/*******************************************************************************
* Function Name: MAX31856_writeReg()
****************************************************************************//**
* \brief
*   Writes to a register on the MAX31856 
*
* \param state [in/out]
*   Pointer to the state struct
*
* \param addr [in]
*   Address of the register
*
* \param val [in]
*   Value of the register 
*  
*
* \return
*   Error code of the operation
*******************************************************************************/
uint32_t MAX31856_writeReg(MAX31856_STATE_S* state, uint8_t addr, uint8_t val){
    /* Ensure state exists */
    uint32_t error = Comms_validateSpi(state->spi);
    if(!error) {
        /* Set slave active */
        error|= state->spi->setActive(state->slaveId);
        /* Places results into val */
        uint32_t spiError = state->spi->write( (addr | MAX31856_MASK_WRITEADDR), val);
        /* Place output error in the state struct */
        error |= spiError ? COMMS_ERROR_SPI : COMMS_ERROR_NONE;
        if(error){state->error = spiError;}
    }
    return error;
}

/*******************************************************************************
* Function Name: MAX31856_readArray()
****************************************************************************//**
* \brief
*   Reads multiple register from the MAX31856 
*
* \param state [in/out]
*   Pointer to the state struct
*
* \param addr [in]
*   Address of the register
*
* \param val [out]
*   Return value of the register 
*  
*
* \return
*   Error code of the operation
*******************************************************************************/
uint32_t MAX31856_readArray(MAX31856_STATE_S* state, uint8_t addr, uint8_t *array, uint8_t len){
    /* Ensure state exists */
    uint32_t error = Comms_validateSpi(state->spi);
    if(!error) {
        /* Set slave active */
        error|= state->spi->setActive(state->slaveId);
        /* Places results into Array */
        uint32_t spiError = state->spi->readArray(addr, array, len);
        /* Place output error in the state struct */
        error |= spiError ? COMMS_ERROR_SPI : COMMS_ERROR_NONE;
        if(error){state->error = spiError;}
    }
    return error;
}

/*******************************************************************************
* Function Name: MAX31856_writeArray()
****************************************************************************//**
* \brief
*   Writes to multiple register from the MAX31856 
*
* \param state [in/out]
*   Pointer to the state struct
*
* \param addr [in]
*   Start Address of the registers
*
* \param val [out]
*   Values to write
*
* \return
*   Error code of the operation
*******************************************************************************/
uint32_t MAX31856_writeArray(MAX31856_STATE_S* state, uint8_t addr, uint8_t *array, uint8_t len){
    /* Ensure state exists */
    uint32_t error = Comms_validateSpi(state->spi);
    if(!error) {
        /* Set slave active */
        error|= state->spi->setActive(state->slaveId);
        /* Places results into Array */
        uint32_t spiError = state->spi->writeArray((addr | MAX31856_MASK_WRITEADDR), array, len);
        /* Place output error in the state struct */
        error |= spiError ? COMMS_ERROR_SPI : COMMS_ERROR_NONE;
        if(error){state->error = spiError;}
    }
    return error;
}

/*******************************************************************************
* Function Name: MAX31856_Start()
****************************************************************************//**
* \brief
*   Sets the device to Automatic conversion mode, four sample average, type K  
*
* \param state [in/out]
*   Pointer to the state struct
*
* \return
*   Error code of the operation
*******************************************************************************/
uint32_t MAX31856_Start(MAX31856_STATE_S* state){
    uint32_t error = MAX31856_writeReg(state, MAX31856_CR0_ADDR, MAX31856_CR0_MASK_CMODE);
    uint8_t CR1_val = (MAX31856_CR1_MASK_TC_K | MAX31856_CR1_MASK_AVG4);
    error |= MAX31856_writeReg(state, MAX31856_CR1_ADDR, CR1_val);
    if(!error) {
        uint8_t readVal;
        error |= MAX31856_readReg(state, MAX31856_CR1_ADDR, &readVal);
        if(error || (readVal != CR1_val)) {
            error = COMMS_ERROR_START;
           state->error = readVal;    
        } 
    }
    return error;
}

/*******************************************************************************
* Function Name: MAX31856_ReadTemp()
****************************************************************************//**
* \brief
*   Executes a read command from the device. Places the result into the
*   'temp' field of the state struct. 
*
* \param state [in/out]
*   Pointer to the state struct
*
* \return
*   Error code of the operation
*******************************************************************************/
uint32_t MAX31856_ReadTemp(MAX31856_STATE_S* state){
    uint8_t tempArray[THREE];
    /* SPI Read data */
    uint32_t error = MAX31856_readArray(state, MAX31856_LTCBH_ADDR, tempArray, THREE);
    if(!error) {
        uint32_t tempBin = 
            (tempArray[ZERO] << BITS_TWO_BYTES) | 
            (tempArray[ONE]  << BITS_ONE_BYTE)  |
            (tempArray[TWO]);
        /* Convert to a float */
        state->temp = MAX31856_ConvertTemp(tempBin);
    }
    return error;
}

/*******************************************************************************
* Function Name: MAX31856_ConvertTemp()
****************************************************************************//**
* \brief
*   Converts a left justified binary temp to a float (°C)
*
* \param state [in]
*   Temperature in binary
*
* \return
*   Temperature in °C
*******************************************************************************/
float MAX31856_ConvertTemp(uint32_t temp){
    uint8_t i;
    float tempF = ZERO_F;
    int8_t power = MAX31856_TEMP_START_POWER;
    for(i = MAX31856_TEMP_SHIFT_LSB; i <= MAX31856_TEMP_SHIFT_MSB; i++){
        if(temp & (ONE << i)){
            /* Avoid needing math.h */
            // tempF += powf(TWO_F, (float) power);
            tempF += power >= 0 ? (float) (1 << power) : (1 / (float) (1<< (-1*power)));
        }
        power++;
    }
    /* Sign Bit */
    if(temp & MAX31856_TEMP_MASK_SIGN){
        tempF = tempF - MAX31856_TEMP_NEG;
    }
    return tempF;
}


/* [] END OF FILE */
