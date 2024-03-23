/***************************************************************************
*                           Majestic Labs  © 2023
* File: template_main_psoc6.c
* Workspace: MJL Driver Templates
* Version: v1.0
* Author: Craig Cheney
*
* PCB: 
*
* Brief:
*   Template for the PSoC6 Main for the CM0+ Core
*
* Change Log:
*   2023.12.31 - Document created
********************************************************************************/
#include "project.h" /* Cypress project */
#include "mjl_errors.h"
#include "mjl_uart.h"
#include "hal_psoc6.h"
#include "LTC6915.h"
#include "DRV8244.h"
#include "max31856.h"


/* ####################### BEGIN PROGRAM CONFIGURATION ###################### */

//#define MJL_DEBUG
/* ---------------- DEBUG CASE ----------------
* Uncomment ONE of the following
* Debugging will only occur when MJL_DEBUG is defined
*/
#ifdef MJL_DEBUG
//  #define MJL_DEBUG_LED            /* Test the battery charger LED */
//  #define MJL_DEBUG_UART            /* Test the UART Connection */
//  #define MJL_DEBUG_LTC     /* Test the LTC6915 SPI implmenetation  */
//  #define MJL_DEBUG_TEMP        /* Test SPI and the MAX31856 temp chip */
//  #define MJL_DEBUG_DRV /* Basic usage the DRV driver chip */
//  #define MJL_DEBUG_PWM /*  Test PWM  */
//  #define MJL_DEBUG_TIMERS /* Test the timers */
//  #define MJL_DEBUG_ADC               /* Read the ADC */
//  #define MJL_DEBUG_ADC_INT          /* Read the ADC using the interrupt flag */
//  #define MJL_DEBUG_ADC_AVG          /* Read the ADC and average in software */
  #define MJL_DEBUG_SYSTEM_COUNTER      /* Test using the microsecond counter */
#endif
/* -------------- END DEBUG CASE --------------  */


/* ############################# BEGIN PROGRAM ############################## */
/* Macro Definitions */
#define PWM_DRV_PERIOD (999)
#define PWM_DRV_DUTY_50  (PWM_DRV_PERIOD+1)/2
#define PWM_DRV_DUTY_100  (PWM_DRV_PERIOD+1)
#define ADC_CHAN_DRV_IPROP (0) /* Current measure of the DRV */
#define ADC_SAMPLES_PER_SEC  (939) /* Rate of the ADC in [Hz] */
#define TEMP_SENSE_ID  (0)
#define DRV_R_IPROPI   (1000.0) /* Shunt resistor value in [Ω] */
#define DRV8244_GAIN_IPROPI_QFN       (4750.0) /* Current gain in [A/A] */
#define DRV_VIPROPI_TO_I  (DRV8244_GAIN_IPROPI_QFN / DRV_R_IPROPI) /* [1/Ω] - Multiply V_ipropi by this to get the current in [A]  */

/* System timer */
typedef struct {
  uint32_t second;
  uint32_t microsecond;
} system_time_s;

uint32_t get_system_time_atomic(system_time_s *const state);

uint32_t drv8244_current_from_voltage(drv8244_state_s *const, float32_t voltage); 

/* Global Variables */
volatile bool flag_timer_second;
volatile bool flag_adc;
MLJ_UART_S usb;
MJL_SPI_S spi;
max31856_state_s tempSense;
drv8244_state_s drv;
system_time_s time;

/* ISR Declarations */
void isr_adc(void);
void isr_second(void);

/* Function Definitions */
uint32_t initHardware(void);
void breatheLed_setState(bool state);

/* Main Program */
#ifndef MJL_DEBUG
/*******************************************************************************
* Function Name: main()
********************************************************************************
* Summary:
*   The top-level application function for the project.
*
*******************************************************************************/
int main(void) {

  initHardware();
  uart_printHeader(&usb, "MJL_TEMPLATE_MAIN", __DATE__, __TIME__);
  CyDelay(10);
  uart_println(&usb, "");
  uart_println(&usb,"* Press 'Enter' to reset");
  /* Enable CM4.  CY_CORTEX_M4_APPL_ADDR must be updated if CM4 memory layout is changed. */
  //  Cy_SysEnableCM4(CY_CORTEX_M4_APPL_ADDR); 
  
  for(;;){
    /* Handle the UART */
    uint8_t readVal = 0;
    uint32_t readError = uart_read(&usb, &readVal);
    if(!readError) {
      /* Reset on Enter */
      if('\r' == readVal) {
        uart_print(&usb, "\r\nResetting...");
        CyDelay(1000);
        /* Reset both cores */
        NVIC_SystemReset();
      }
    }
  }    
}
#endif /* ifndef MJL_DEBUG*/
/* End Main Program */


/* ############################ BEGIN DEBUGGER ############################## */
#ifdef MJL_DEBUG
/*******************************************************************************
* DEBUGGER: main()
********************************************************************************
* Summary:
*   Debugging function for the PCB hardware
*******************************************************************************/
int main(void){
  #warning "MJL_DEBUG is enabled"
  initHardware();
  
  /* Test Cases */
  /* Test Cases */
  #ifdef MJL_DEBUG_LED
    /* blink the LED */
    for(;;) {
      Cy_GPIO_Write(pin_LED_G_PORT, pin_LED_G_NUM, LED_ON);
      Cy_GPIO_Write(pin_LED_R_PORT, pin_LED_R_NUM, LED_OFF);
      CyDelay(500);
      Cy_GPIO_Write(pin_LED_G_PORT, pin_LED_G_NUM, LED_OFF);
      Cy_GPIO_Write(pin_LED_R_PORT, pin_LED_R_NUM, LED_ON);
      CyDelay(500);
    }
  /* End MJL_DEBUG_LED */
  #elif defined MJL_DEBUG_UART             
  /* Test the UART Connection */
  uart_printHeader(&usb, "MJL_DEBUG_UART", __DATE__, __TIME__);
  CyDelay(10);
  uart_println(&usb, "");
  uart_println(&usb,"* Press 'Enter' to reset");
  uart_println(&usb, "");

  for(;;) {
    uint8_t readVal = 0;
    uint32_t readError = uart_read(&usb, &readVal);
    if(!readError) {
      /* Echo UART */
      uart_write(&usb, readVal);                
      /* Reset on Enter */
      if('\r' == readVal) {
        uart_print(&usb, "\r\nResetting...");
        CyDelay(1000);
        /* Reset both cores */
        NVIC_SystemReset();
      }
    }
  }
  #elif defined MJL_DEBUG_LTC     
    /* Test the LTC6915 SPI implmenetation  */
    uart_printHeader(&usb, "MJL_DEBUG_LTC", __DATE__, __TIME__);
    CyDelay(10);
    uart_println(&usb, "");
    uart_println(&usb,"* Press 'Enter' to reset");
    uart_println(&usb,"* Press '0' to toggle gain between 0 and 1");
    uart_println(&usb,"* Press 'Space' to increase the gain");
    uart_println(&usb, "");
    Cy_GPIO_Write(pin_LED_G_PORT, pin_LED_G_NUM, LED_ON);
    
    /* Configure the LTC structure */
    LTC6915_S ltc;
    LTC6915_cfg_s ltcCfg = ltc6915_cfg_default;
    ltcCfg.spi = &spi;
    ltcCfg.slaveId = 0;
    ltcCfg.mode = LTC6915_MODE_SERIAL;
    uint32_t error =0;
    uart_printlnf(&usb, "SPI Init: %b", spi._init);
    
    error |= ltc6915_init(&ltc, &ltcCfg);
    uart_printError(&usb, "LTC INIT", error);
    if(!error){
      error |= ltc6915_start(&ltc);
      uart_printError(&usb, "LTC Start", error);
    }
    uart_println(&usb, "");
    
    bool isGain1 = false;
    for(;;) {
      uint8_t readVal = 0;
      uint32_t readError = uart_read(&usb, &readVal);
      if(!readError) {  
        /* Reset on Enter */
        if('\r' == readVal) {
          uart_print(&usb, "\r\nResetting...");
          CyDelay(1000);
          /* Reset both cores */
          NVIC_SystemReset();
        }
        /* Toggle gain between 0 and 1 */
        else if ('0' == readVal){
          isGain1 = !isGain1;
          uart_printlnf(&usb, "LTC enabled: %b", isGain1);
          LTC6915_GAIN_T gainWord = isGain1 ? LTC6915_GAIN_1: LTC6915_GAIN_0;
          ltc6915_setGainWord(&ltc, gainWord);
        }
        /* Increase the gain */
        else if (' ' == readVal){
          LTC6915_GAIN_T gainWord = 0;
          uint16_t gain =0;
          ltc6915_getNextGainWord(&ltc, &gainWord);
          ltc6915_valueFromWord(gainWord, &gain);
          ltc6915_setGainWord(&ltc, gainWord);
          uart_printlnf(&usb, "New gain: %d, Word: %d", gain, gainWord); 
        }
      }
    }
    #elif defined MJL_DEBUG_TEMP        
    /* Test SPI and the MAX31856 temp chip */
    uart_printHeader(&usb, "MJL_DEBUG_TEMP", __DATE__, __TIME__);
    CyDelay(10);
    uart_println(&usb, "");
    uart_println(&usb,"* Press 'Enter' to reset");
    uart_println(&usb, "");
    /* Ensure interrups are enabled */    
    
    for(;;) {
      max31856_readTemp(&tempSense);
      uart_printlnf(&usb, "Temp: %.2f", tempSense.temp);
      CyDelay(1000);
      
      uint8_t readVal = 0;
      uint32_t readError = uart_read(&usb, &readVal);
      if(!readError) {
        /* Echo UART */
        uart_write(&usb, readVal);                
        /* Reset on Enter */
        if('\r' == readVal) {
          uart_print(&usb, "\r\nResetting...");
          CyDelay(1000);
          /* Reset both cores */
          NVIC_SystemReset();
        }
      }
    }
  #elif defined MJL_DEBUG_DRV 
    /* Basic usage the DRV driver chip */
    uart_printHeader(&usb, "MJL_DEBUG_DRV", __DATE__, __TIME__);
    CyDelay(10);
    uart_println(&usb, "");
    uart_println(&usb,"* Press 'Enter' to reset");
    uart_println(&usb,"* Press '0' to toggle START/STOP");
    uart_println(&usb,"* Press 'space' to toggle ENABLE");
    uart_println(&usb, "");
    
    for(;;) {
    uint8_t readVal = 0;
    uint32_t readError = uart_read(&usb, &readVal);
    if(!readError) {
      /* Echo UART */
      uart_write(&usb, readVal);                
      /* Reset on Enter */
      if('\r' == readVal) {
        uart_print(&usb, "\r\nResetting...");
        CyDelay(1000);
        /* Reset both cores */
        NVIC_SystemReset();
      }
      /* Enable the DRV */
      else if (' ' == readVal){
        if(drv.isEnabled){
          uart_println(&usb, "Disabling the DRV");
          drv8244_disable(&drv);
        }
        else {
          uart_println(&usb, "Enabling the DRV");
          drv8244_enable(&drv);
        }
      }
      /* START/STOP the DRV */
      else if ('0' == readVal) {
        if(!drv._running){
          uart_println(&usb, "Starting the DRV");
          drv8244_start(&drv);
        }
        else {
          uart_println(&usb, "Stopping the DRV");
          drv8244_stop(&drv);
        }
      }
      
//      bool in1 = Cy_GPIO_ReadOut(pin_DRV_DEBUG_PWM_PORT, pin_DRV_DEBUG_PWM_0_NUM);
      bool in1 = false;
      bool dir = Cy_GPIO_ReadOut(pin_DRV_DIR_PORT, pin_DRV_DIR_0_NUM);
      bool drvoff = Cy_GPIO_ReadOut(pin_DRV_DRVOFF_PORT, pin_DRV_DRVOFF_0_NUM);
      bool nSleep = Cy_GPIO_ReadOut(pin_DRV_nSLEEP_PORT, pin_DRV_nSLEEP_0_NUM);
      bool nFault = Cy_GPIO_Read(pin_DRV_nFAULT_PORT, pin_DRV_nFAULT_0_NUM);
      uart_printlnf(&usb, "IN1: %b, IN2: %b, AWAKE: %b, ACTIVE: %b, FAULT: %b", \
          in1, dir, nSleep, !drvoff, !nFault);
    }
  }
  #elif defined MJL_DEBUG_PWM           
    /* Basic usage the DRV driver chip */
    uart_printHeader(&usb, "MJL_DEBUG_DRV", __DATE__, __TIME__);
    CyDelay(10);
    uart_println(&usb, "");
    uart_println(&usb, "* Press ' ' to toggle PWM");
    uart_println(&usb, "");
          
    bool pwmState = false;
    bool prevPwmState = true;
        
    for(;;) {
      /* Check for UART input */
      uint8_t readVal = 0;
      uint32_t readError = uart_read(&usb, &readVal);
      if(readVal == ' ') {
          pwmState = !pwmState;
      }
      /* Detect State Change */
      if(pwmState != prevPwmState) {
          prevPwmState = pwmState;
          uart_println(&usb, CMD_CLEAR_LINEUP);
          if(pwmState) {
              PWM_DRV_Enable();
              Cy_TCPWM_TriggerStart(PWM_DRV_HW, PWM_DRV_CNT_MASK);
              breatheLed_setState(true);
          }
          else {
              PWM_DRV_Disable();
              breatheLed_setState(false);
          }
          uart_printlnf(&usb, "PWM Enabled: %b", pwmState);
        }
    }
  /* End MJL_DEBUG_PWM */
  #elif defined MJL_DEBUG_TIMERS 
    /* Test the timers */
        /* Basic usage the DRV driver chip */
    uart_printHeader(&usb, "MJL_DEBUG_DRV", __DATE__, __TIME__);
    CyDelay(10);
    uart_println(&usb, "");
    uart_println(&usb, "");
         
    for(;;) {
      /* Handle Print timer */
      if(flag_timer_second) {
        flag_timer_second = false;
        uart_println(&usb, "a");
      }
      /* Handle temperature error */
      uint8_t readVal = 0;
      uint32_t readError = uart_read(&usb, &readVal);
      if(!readError) {  
        /* Reset on Enter */
        if('\r' == readVal) {
          uart_print(&usb, "\r\nResetting...");
          CyDelay(1000);
          /* Reset both cores */
          NVIC_SystemReset();
        }
      }
    }
  #elif defined MJL_DEBUG_ADC          
    /* Start and read the ADC */
    uart_printHeader(&usb, "MJL_DEBUG_ADC", __DATE__, __TIME__);
    CyDelay(10);
    uart_println(&usb, "\r\n");
    
    ADC_Start();
    ADC_StartConvert();
    
    for(;;) {
        ADC_IsEndConversion(CY_SAR_WAIT_FOR_RESULT);
        uint16_t val = ADC_GetResult16(ADC_CHAN_DRV_IPROP);
        uart_printlnf(&usb, "ADC: 0x%x (%d)", val, val);
        CyDelay(1000);
    }
        
    /* End MJL_DEBUG_ADC */
    #elif defined MJL_DEBUG_ADC_INT          
      /* Read the ADC using the interrupt flag */
      uart_printHeader(&usb, "MJL_DEBUG_ADC_INT", __DATE__, __TIME__);
      CyDelay(10);
      uart_println(&usb, "\r\n");
      
      /* Start the ADC Conversions */
      Cy_SAR_StartConvert(ADC_SAR__HW, CY_SAR_START_CONVERT_CONTINUOUS);
      
      uint16_t adcVal = 0;
      for(;;) {
          /* ADC is available for servicing */
          if(flag_adc) {
              flag_adc = false;
              adcVal = ADC_GetResult16(ADC_CHAN_DRV_IPROP);
          }
      }
    /* End MJL_DEBUG_ADC */ 
  #elif defined MJL_DEBUG_ADC_AVG          
    /* Read the ADC and average */
    uart_printHeader(&usb, "MJL_DEBUG_ADC_AVG", __DATE__, __TIME__);
    CyDelay(10);
    uart_println(&usb, "\r\n");
      
    uint32_t ongoingSum = 0;
    uint16_t iteration = 0;
    const uint16_t samplesPerSecond = 939; 
    
    /* Start the ADC Conversions */
    Cy_SAR_StartConvert(ADC_SAR__HW, CY_SAR_START_CONVERT_CONTINUOUS);

    for(;;) {
      /* ADC is available for servicing */
      if(flag_adc) {
        flag_adc = false;
        uint16_t adcVal = ADC_GetResult16(ADC_CHAN_DRV_IPROP);
        ongoingSum += adcVal;
        iteration++;
        if(samplesPerSecond <=  iteration) {
          Cy_GPIO_Write(pin_DEBUG_ADC_SUM_PORT, pin_DEBUG_ADC_SUM_NUM, 1);
          uint32_t mean = ongoingSum/iteration;
          ongoingSum = 0;
          iteration = 0;
          uart_printlnf(&usb, "%d", mean);
          Cy_GPIO_Write(pin_DEBUG_ADC_SUM_PORT, pin_DEBUG_ADC_SUM_NUM, 0);
        }
      }
    } 
    /* End MJL_DEBUG_ADC_AVG */ 
  #elif defined MJL_DEBUG_SYSTEM_COUNTER      
    /* Test using the minute counter */
    uart_printHeader(&usb, "MJL_DEBUG_SYSTEM_COUNTER", __DATE__, __TIME__);
    CyDelay(10);
    uart_println(&usb, "");
    uart_println(&usb,"* Press 'Enter' to reset");
    uart_println(&usb, "");
    
    for(;;) {
      /* Triggered every second */
      if(flag_timer_second){
        flag_timer_second = false;
        get_system_time_atomic(&time);
        uart_printlnf(&usb, "sec:%d, micro:%d", time.second, time.microsecond);
      }
      /* Handle the UART */
      uint8_t readVal = 0;
      uint32_t readError = uart_read(&usb, &readVal);
      if(!readError) {
        /* Echo UART */
        //uart_write(&usb, readVal);                
        /* Reset on Enter */
        if('\r' == readVal) {
          uart_print(&usb, "\r\nResetting...");
          CyDelay(1000);
          /* Reset both cores */
          NVIC_SystemReset();
        }
      }
    }
  #endif 
  /* Fall through Infinite loop */
  for(;;){}
}
#endif /* MJL_DEBUG */


/* ####################### BEGIN FUNCTION DEFINITIONS ####################### */

/*******************************************************************************
* Function Name: initHardware()
********************************************************************************
* \brief
*   Configures the hardware to start
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t initHardware(void) {
  uint32_t error = 0;
  /* Start the UART */
  MJL_UART_CFG_S uartCfg = uart_cfg_default;
  uartCfg.hal_req_writeArray = uart_psoc6SCB_writeArrayBlocking;
  uartCfg.hal_req_read = uart_psoc6SCB_read;
  uartCfg.hal_opt_externalStart = uart_psoc6SCB_start;
  uartCfg.hal_opt_externalStop = uart_psoc6SCB_stop;
  error |= uart_init(&usb, &uartCfg);
  error |= uart_start(&usb);
    
  /* Start the SPI Component */
  MJL_SPI_CFG_S spiCfg = spi_cfg_default;
  spiCfg.opt_hal_externalStart = spi_psoc6SCB_start;
  spiCfg.opt_hal_externalStop = spi_psoc6SCB_stop;
  spiCfg.req_hal_writeArray_blocking = spi_psoc6SCB_writeArray_blocking;
  spiCfg.req_hal_read = spi_psoc6SCB_read;
  spiCfg.req_hal_setActive = spi_psoc6SCB_setActive;
  spiCfg.req_hal_getRxBufferNum = spi_psoc6SCB_getRxBufferNum;
  spiCfg.req_hal_getTxBufferNum = spi_psoc6SCB_getTxBufferNum;
  spiCfg.req_hal_clearRxBuffer = spi_psoc6SCB_clearRxBuffer;
  spiCfg.req_hal_clearTxBuffer = spi_psoc6SCB_clearTxBuffer;
  error |= spi_init(&spi, &spiCfg);
  error |= spi_start(&spi);
  
  /* Start the DRV8244 */
  drv8244_cfg_s drvCfg = drv8244_config_default;
  drvCfg.fn_delay_us = Cy_SysLib_DelayUs;
  drvCfg.fn_criticalSection_enter = Cy_SysLib_EnterCriticalSection;
  drvCfg.fn_criticalSection_exit = Cy_SysLib_ExitCriticalSection;    
  drvCfg.fn_pin_sleep_write = hal_drv_pin_sleep_write;
  drvCfg.fn_pin_drvoff_write = hal_drv_pin_drvoff_write;
  drvCfg.fn_pin_fault_read = hal_drv_pin_nfault_read;
  uint32_t error_drv = drv8244_init(&drv, &drvCfg);
  error_drv |= drv8244_start(&drv);
  uart_printError(&usb, "drv8244 Init & start", error_drv);
  
  /* Start the temperature sensor */
  max31856_cfg_s tempSenseCfg = max31856_config_default;
  tempSenseCfg.slaveId = TEMP_SENSE_ID;
  tempSenseCfg.spi = &spi;
  uint32_t error_temp = 0;
  error_temp |= max31856_init(&tempSense, &tempSenseCfg);
  error_temp |= max31856_start(&tempSense);
  uart_printError(&usb, "Start Temp sense", error_temp);
  if(error_temp){
    uint8_t CR1_val = (MAX31856_CR1_MASK_TC_K | MAX31856_CR1_MASK_AVG16);
    uart_printlnf(&usb, "Read 0x%x, expected 0x%x",tempSense.error,CR1_val); 
  }
  error |= error_temp;
  
  /* Start & disable the DRV PWM */
  PWM_DRV_Start();
  PWM_DRV_Disable();
  /* Start and disable Breating PWM */
  pwm_breath1_Start();
  pwm_breath2_Start();
  breatheLed_setState(false);
  
  /*Connect the microsecond timer to the second interrupt */
  Cy_SysInt_Init(&interrupt_second_cfg, isr_second);
  NVIC_ClearPendingIRQ(interrupt_second_cfg.intrSrc);
  NVIC_EnableIRQ(interrupt_second_cfg.intrSrc);
  counter_microsecond_Start();
  counter_second_Start();
  
  /* Enable the ADC  */
  Cy_SysInt_Init(&interrupt_adc_cfg, isr_adc);
  NVIC_ClearPendingIRQ(interrupt_adc_cfg.intrSrc);
  NVIC_EnableIRQ(interrupt_adc_cfg.intrSrc);
  ADC_Start();
  NVIC_EnableIRQ(ADC_IRQ_cfg.intrSrc);
  
  /* Start global interrupts */
  __enable_irq(); /* Enable global interrupts. */
  
  return error;
}

/*******************************************************************************
* Function Name: breatheLed_setState()
********************************************************************************
* \brief
*   Start or stop the breathing LED
* 
* \param state
*   Boolean state to enter
*
* \return
*  None
*******************************************************************************/
void breatheLed_setState(bool state){
    /* Breathe */
    if(state) {
        pwm_breath1_Enable();
        pwm_breath2_Enable();
        Cy_TCPWM_TriggerStart(pwm_breath1_HW, pwm_breath1_CNT_MASK);
        Cy_TCPWM_TriggerStart(pwm_breath2_HW, pwm_breath2_CNT_MASK);
    } 
    /* Stop Breathe */
    else {
        pwm_breath1_Disable();
        pwm_breath2_Disable();
    }
}
/*******************************************************************************
* Function Name: isr_second()
********************************************************************************
* \brief
*   Triggers after 1 second
*
*******************************************************************************/
void isr_second(void){
  flag_timer_second = true;
  counter_microsecond_ClearInterrupt(CY_TCPWM_INT_ON_TC);
  NVIC_ClearPendingIRQ(interrupt_second_cfg.intrSrc);
}


/*******************************************************************************
* Function Name: isr_adc()
********************************************************************************
* \brief
*   Triggers when the ADC End of Scan is complete (EOS)
*
*******************************************************************************/
void isr_adc(void) {
  flag_adc = true;
  NVIC_ClearPendingIRQ(interrupt_adc_cfg.intrSrc);
}

/*******************************************************************************
* Function Name: get_system_time_atomic()
********************************************************************************
* \brief
*   Atomically updates the system time state structure passed in 
*
*******************************************************************************/
uint32_t get_system_time_atomic(system_time_s *const state){
 uint32_t error = 0;
  /* Get atomically */
  uint8_t intr_state = Cy_SysLib_EnterCriticalSection();
  state->microsecond = Cy_TCPWM_Counter_GetCounter(counter_microsecond_HW, counter_microsecond_CNT_NUM);
  state->second = Cy_TCPWM_Counter_GetCounter(counter_second_HW, counter_second_CNT_NUM);
  Cy_SysLib_ExitCriticalSection(intr_state);
  return error;
}

/* [] END OF FILE */
