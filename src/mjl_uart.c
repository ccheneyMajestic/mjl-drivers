/***************************************************************************
*                                Majestic Labs © 2023
* File: mjl_uart.c
* Workspace: MJL Driver Library 
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: Serial communication library for the MJL driver library 
*   
*
* 2023.04.26  - Document Created
********************************************************************************/
#include "mjl_uart.h"
#include <stdarg.h>
#include <stddef.h>
#include "float.h"

static const double pow10[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };
const uint8_t hexAscii[HEX_VAL_MAX+1] = "0123456789ABCDEF";


/* Default config struct */
const MJL_UART_CFG_S uart_cfg_default = {
  .fn_writeArray = NULL,
  .fn_read = NULL,
  .fn_externalStart = NULL,
  .baud = 0,
};

/*******************************************************************************
* Function Name: uart_init()
********************************************************************************
* \brief
*   Initializes the uart state struct from a configuration struct 
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
uint32_t uart_init(MLJ_UART_S *const state, MJL_UART_CFG_S *const cfg){
  uint32_t error = 0;
  /* Verify required functions */
  error |= (NULL == cfg->fn_writeArray) ? ERROR_POINTER : ERROR_NONE;
  error |= (NULL == cfg->fn_read) ? ERROR_POINTER : ERROR_NONE;
  /* external start is not required */
  /* external stop is not required */
  error |= (0 == cfg->baud) ? ERROR_VAL : ERROR_NONE;
  /* Valid Inputs */
  if(!error) {
    /* Copy params */
    state->fn_writeArray = cfg->fn_writeArray;
    state->fn_read = cfg->fn_read;
    state->fn_externalStart = cfg->fn_externalStart;
    state->baud = cfg->baud;
    /* Mark as initialized */
    state->_init = true;
    state->_running = false;
    state->isLoggingEnabled = true;
  }
  if(error){state->_init=false;}
  return error;
}

/*******************************************************************************
* Function Name: uart_start()
********************************************************************************
* \brief
*   Starts the UART and call the external start function, if present 
*
* \param state [in/out]
* Pointer to the state struct
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t uart_start(MLJ_UART_S *const state){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(state->_running){error|=ERROR_RUNNING;}

  if(!error){
    /* Pre-mark as running */
    state->_running = true;
    /* Run the external start function if present  */
    if(NULL != state->fn_externalStart){
      error |= state->fn_externalStart(state->baud);
    }
  }
  return error;
}

/*******************************************************************************
* Function Name: uart_stop()
********************************************************************************
* \brief
*   Stops the uart 
*
* \param state [in/out]
* Pointer to the state struct
*
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t uart_stop(MLJ_UART_S *const state){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(!state->_running){error|=ERROR_STOPPED;}

  if(!error){
    /* Run the external stop function if present  */
    if(NULL != state->fn_externalStop){
      state->fn_externalStop();
    }
    state->_running = false;
  }
  return error;
}

/*******************************************************************************
* Function Name: uart_write()
********************************************************************************
* \brief
*   Writes a single byte of data out the uart 
*
* \param state [in/out]
* Pointer to the state struct
*
* \param data [in]
* Value to write 
* 
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t uart_write(MLJ_UART_S *const state, uint8_t data){
  return uart_writeArray(state, &data, 1);
}

/*******************************************************************************
* Function Name: uart_read()
********************************************************************************
* \brief
*   Reads a single byte of data in form the uart 
*
* \param state [in/out]
* Pointer to the state struct
*
* \param data [out]
* L:ocation to place the data 
* 
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t uart_read(MLJ_UART_S *const state, uint8_t * data){
  return uart_readArray(state, data, 1);
}

/*******************************************************************************
* Function Name: uart_writeArray()
********************************************************************************
* \brief
*   Writes an array out of the uart
*
* \param state [in/out]
* Pointer to the state struct
*
* \param array [in]
* Pointer to the data
*
* \param len [in]
* Number of characters to write 
* 
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t uart_writeArray(MLJ_UART_S *const state, uint8_t * array, uint16_t len){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(!state->_running){error|=ERROR_STOPPED;}
  if(!state->isLoggingEnabled){error|=ERROR_MODE;}

  if(!error){
    error|= state->fn_writeArray(array, len);
  }
  return error;
}

/*******************************************************************************
* Function Name: uart_readArray()
********************************************************************************
* \brief
*   Reads in data from the UART 
*
* \param state [in/out]
* Pointer to the state struct
*
* \param array [out]
* Pointer to the data
*
* \param len [in]
* Number of characters to read 
* 
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t uart_readArray(MLJ_UART_S *const state, uint8_t * array, uint16_t len){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(!state->_running){error|=ERROR_STOPPED;}

  if(!error){
    for(uint16_t i=0; i<len; i++){
      error |= state->fn_read(&array[i]);
      if(error){break;}
    }
  }
  return error;
};


/*******************************************************************************
* Function Name: uart_write_reverse()
********************************************************************************
* \brief
*   Writes an array out of the uart in reverse order 
*
* \param state [in/out]
* Pointer to the state struct
*
* \param array [in]
* Pointer to the data
*
* \param len [in]
* Number of characters to write 
* 
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t uart_write_reverse(MLJ_UART_S *const state, uint8_t * array, uint16_t len){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(!state->_running){error|=ERROR_STOPPED;}
  if(!state->isLoggingEnabled){error|=ERROR_MODE;}

  if(!error){
    for(uint16_t i = len; i >0; i--){
      error|= state->fn_writeArray(&array[i-1], 1);
      if(error){break;}
    }
  }
  return error;
}

/*******************************************************************************
* Function Name: uart_print()
********************************************************************************
* \brief
*   Prints a zero terminated string out on the uart 
*
* \param state [in/out]
* Pointer to the state struct
*
* \param pszFmt [in]
* Pointer to a zero-terminated string
* 
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t uart_print(MLJ_UART_S* state, const char *pszFmt) {
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(!state->_running){error|=ERROR_STOPPED;}
  if(!state->isLoggingEnabled){error|=ERROR_MODE;}

  if(!error){
    /* Find the length */
    uint16_t len=0;
    while(0 != pszFmt[len]){len++;}
    state->fn_writeArray((uint8_t *) pszFmt, len);
  }
  return error;
}

/*******************************************************************************
* Function Name: uart_println()
********************************************************************************
* \brief
*   Prints a zero terminated string out on the uart followed by a newline and 
* Carriage return 
*
* \param state [in/out]
* Pointer to the state struct
*
* \param pszFmt [in]
* Pointer to a zero-terminated string
* 
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t uart_println(MLJ_UART_S *const state, const char * pszFmt){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(!state->_running){error|=ERROR_STOPPED;}
  if(!state->isLoggingEnabled){error|=ERROR_MODE;}

  if(!error){
    error |= uart_print(state, pszFmt);
    error |= uart_print(state, "\r\n");
  }
  return error;
}


/*******************************************************************************
* Function Name: uart_printlnf()
********************************************************************************
* \brief
*   Prints a formatted zero terminated string out on the uart followed by a newline and 
* Carriage return 
*
* \param state [in/out]
* Pointer to the state struct
*
* \param pszFmt [in]
* Pointer to a zero-terminated string
* 
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t uart_printlnf(MLJ_UART_S* state, const char *pszFmt,...){
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(!state->_running){error|=ERROR_STOPPED;}
  if(!state->isLoggingEnabled){error|=ERROR_MODE;}

  if(!error){
    va_list args;
    va_start(args, pszFmt);
    error |= uart_printf(state, pszFmt, args);
    error |= uart_print(state, "\r\n");
    va_end(args);
  }
  return error;
}

/*******************************************************************************
* Function Name: uart_printf()
********************************************************************************
* \brief
*   Prints a character out on the uart 
*
* \param state [in/out]
* Pointer to the state struct
*
* \param pszFmt [in]
* Pointer to a zero-terminated string
* 
* \return
*  Error code of the operation
*******************************************************************************/
uint32_t uart_printf(MLJ_UART_S* state, const char *pszFmt,...) {
  uint32_t error = 0;
  if(!state->_init){error|=ERROR_INIT;}
  if(!state->_running){error|=ERROR_STOPPED;}
  if(!state->isLoggingEnabled){error|=ERROR_MODE;}

  if(!error){
    va_list args;
    va_start(args, pszFmt);

    while(*pszFmt) {
      /* Print until the format specifier is encountered */
      if('%' != *pszFmt) {
        error |= state->fn_writeArray((uint8_t *) pszFmt, 1);
        pszFmt++;
        continue;
      }
      /* Skip the '%' */
      pszFmt++;
      /* Format a string */
      if(*pszFmt == 's') {
        /* Get the pointer to the string */
        char* str = (char*) va_arg(args, int);
        for(; *str != '\0'; str++){
          error |= state->fn_writeArray((uint8_t *)str,1);
        }
        pszFmt++;
        continue;
      }

      /* Boolean */
      else if(*pszFmt == 'b'){
        bool bVal = (bool) va_arg(args, int);
        if(bVal){
          error |= uart_print(state, "True");
        } else {
          error |= uart_print(state, "False");
        }
        pszFmt++;
        continue;
      }
      /* Format integer */
      else if(*pszFmt == 'd' || *pszFmt == 'i') {
        int iVal = va_arg(args, int);
        uint8_t i = 0;
        uint8_t buffer[25];
        do{
          buffer[i++] = iVal % 10;
          iVal /= 10;
        }while(iVal);
        while(i > 0){
          i--;
          uint8_t ascii = 0;
          error |= uart_hex2Ascii(buffer[i], &ascii);
          error |= state->fn_writeArray(&ascii,1);
        }
        pszFmt++;
        continue;
      }
      /* Character */
      else if(*pszFmt == 'c') {
        uint8_t cVal = (uint8_t) va_arg(args, int);
        error |= state->fn_writeArray(&cVal,1);
        pszFmt++;
        continue;
      }
      /* Hex val - 16 bits */
      else if(*pszFmt == 'x') {
        uint16_t hexVal = (uint16_t) va_arg(args, int);
        uint32_t i = 0;
        uint32_t buffer[12] = {0};

        do{
          buffer[i++] = hexVal % HEX_VAL_MAX;
          hexVal /= HEX_VAL_MAX;
        }while(hexVal);
        if(i%2!=0){
          buffer[i++]=0;
        }
        if(i<2) {
          buffer[i++]=0;
        }
        while(i > 0){
          i--;
          uint8_t ascii = 0;
          error |= uart_hex2Ascii(buffer[i], &ascii);
          error |= state->fn_writeArray(&ascii,1);
        }
        pszFmt++;
        continue;
      }

      /* Hex val - 32 bits */
      else if(*pszFmt == 'X') {
        uint32_t hexVal = (uint32_t) va_arg(args, long);
        uint32_t i = 0;
        uint32_t buffer[12] = {0};

        do{
          buffer[i++] = hexVal % HEX_VAL_MAX;
          hexVal /= HEX_VAL_MAX;
        }while(hexVal);
        if(i%2!=0){
          buffer[i++]=0;
        }
        if(i<2) {
          buffer[i++]=0;
        }
        while(i > 0){
          i--;
          uint8_t ascii = 0;
          error |= uart_hex2Ascii(buffer[i], &ascii);
          error |= state->fn_writeArray(&ascii,1);
        }
        pszFmt++;
        continue;
      }
      /* Float */
      else if(*pszFmt == 'f' || *pszFmt == '.') {
        /* Get the precision */
        uint8_t prec = UART_FLOAT_DEFAULT_PREC;
        if(*pszFmt == '.'){
          ++pszFmt;
          prec = *pszFmt - '0';
          ++pszFmt;
        }
        uint8_t buf[UART_FLOAT_BUFFER_SIZE];
        size_t len =0u;
        double diff = 0.0;
        double val = va_arg(args, double);


        /* Test for special values */
        if(val != val){
          uart_print(state, "NaN");
          pszFmt++;
          continue;
        }
        else if (val < -DBL_MAX){
          uart_print(state, "fni-");
          pszFmt++;
          continue;
        }
        else if (val > DBL_MAX) {
          uart_print(state, "fni+");
          pszFmt++;
          continue;
        }
        /* Test for large numbers */
        if( (val > UART_MAX_FLOAT) || (val < -UART_MAX_FLOAT) ) {
          uart_print(state, "*reqEXP");
          pszFmt++;
          continue;
        }
        /* Test for negative */
        bool neg = false;
        if(val < 0){
          neg = true;
          val = 0-val;
        }
        /* Ignore precision for now */
        while((len < UART_FLOAT_BUFFER_SIZE) && (prec > 9u)){
          buf[len++] = '0';
          prec--;
        }

        int whole = (int) val;
        double tmp = (val - whole) * pow10[prec];
        unsigned long frac = (unsigned long) tmp;
        diff = tmp - frac;

        if(diff > 0.5){
          ++frac;
          /* Handle rollover, e.g. case 0.99 with prec 1 is 1.0 */
          if(frac >= pow10[prec]){
            frac = 0;
            ++whole;
          }
        }
        else if (diff < 0.5) {}
        else if ((frac == 0u) || (frac &1u)){
          /* if halfware round up if odd or if last digit is 0 */
          ++frac;
        }

        if(prec == 0u){
          diff = val - (double) whole;
          if((!(diff < 0.5) || (diff > 0.5)) && (whole & 1)) {
            /* Exactly 0.5 and ODD, round up */
            /* 1.5 -> 2, 2.5->2 */
            ++whole;
          }
        }
        else {
          unsigned int count  = prec;
          /* Compute fractional part */
          while (len < UART_FLOAT_BUFFER_SIZE) {
            --count;
            buf[len++] = (char) (48u + (frac % 10u));
            if(!(frac /= 10u)){
              break;
            }
          }
          /* Add extra 0's */
          while ((len < UART_FLOAT_BUFFER_SIZE) && (count-- > 0u)){
            buf[len++] = '0';
          }
          if(len < UART_FLOAT_BUFFER_SIZE){
            /* Add decimal */
            buf[len++] = '.';
          }
        }

        /* Do whole part, number is reversed */
        while(len < UART_FLOAT_BUFFER_SIZE) {
          buf[len++] = (char) (48 + (whole % 10));
          if(!(whole /=10)){
            break;
          }
        }

        if(len < UART_FLOAT_BUFFER_SIZE){
          if(neg){
            buf[len++] = '-';
          }
        }

        /* Print out */
        error |= uart_write_reverse(state, buf, len);
        pszFmt++;
        continue;
      }
      /* End of string */
      if(pszFmt == '\0'){
        break;
      }
    }
    va_end(args);
  }

  return error;
}


/*******************************************************************************
* Function Name: uart_printHeader()
****************************************************************************//**
*
* \brief Displays the program header with Date and time option. Use the macro 
*   'printHeader' for automatically filling date and time. 
* 
* \param name
*    Name of the program to display 
*
* \param time
*    Pass in __TIME__ macro
*
* \param date
*    Pass in __DATE__ macro
*
* \return
* The error code of the operation
*******************************************************************************/
uint32_t uart_printHeader(MLJ_UART_S* state, const char* name, const char *date, const char* time){
  uint32_t error = 0;
  error |= uart_print(state, CLEAR_SCREEN_CMD);
  error |= uart_println(state, "**************************************");
  error |= uart_printf(state, "* %s\r\n*\r\n* Compiled at: %s on %s", name, time, date);
  error |= uart_print(state, "\r\n**************************************\r\n");
  return error;
}

/*******************************************************************************
* Function Name: uart_hex2Ascii()
****************************************************************************//**
* \brief Changes a binary value to a hex ascii character 
*
* \param hex [in]
*  binary value of the hex digit (0-15)
*
* \param ascii [out]
*  Ascii value of the hex digit ('0' - 'F')
*
* \return
*   The error code of the operation
 *******************************************************************************/
uint32_t uart_hex2Ascii(uint8_t hex, uint8_t* ascii){
  uint32_t error = 0;
  if(hex > HEX_VAL_MAX){
    error = ERROR_VAL;
  } else {
    *ascii = hexAscii[hex];
  }
  return error;
}

/* [] END OF FILE */
