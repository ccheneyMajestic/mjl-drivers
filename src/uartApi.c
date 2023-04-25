/***************************************************************************
*                                 MICA  © 2019
*
* File: uartApi.c
* Workspace: micaOS
* Version: v2.0.0
* Author: Craig Cheney
*
* Brief:
*   Conversions for writing out over the serial port. Adapts floating point from 
*   https://github.com/mpaland/printf/blob/master/printf.c
*
* 2019.10.01  - Document Created
********************************************************************************/
#include "uartApi.h"
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <float.h>
#ifdef USE_BUILD_NUMBER
  #include "buildNumber.h"
#endif

uint8_t hexAscii[HEX_VAL_MAX+1] = "0123456789ABCDEF";
/* Powers of 10 */
static const double pow10[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };

#define UART_FLOAT_BUFFER_SIZE    (32u)
#define UART_MAX_FLOAT            (1e9)
#define UART_FLOAT_DEFAULT_PREC   (6u)
/*******************************************************************************
* Function Name: uart_changeBase()
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
  uint32_t error = COMMS_ERROR_NONE;
  if(hex > HEX_VAL_MAX){
    error = COMMS_ERROR_VAL;
  } else {
    *ascii = hexAscii[hex];
  }
  return error;
}

/*******************************************************************************
* Function Name: print()
****************************************************************************//**
*
* \brief Prints out the string
*
* \param pszFmt
*   Pointer to string, zero formatted. String to print out, accepts %d, %s, %x, %c
*
* \return
* The error code of the operation
 *******************************************************************************/
uint32_t print(COMMS_UART_S* uart, const char *pszFmt,...){
  uint32_t error = COMMS_ERROR_NONE;
  /* Validate  state */
  error = Comms_validateUart(uart);
  if(!error) {
    va_list args;
    va_start(args, pszFmt);

    while(*pszFmt) {
      if('%' != *pszFmt) {
          error |= uart->write(*pszFmt);
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
          error |= uart->write(*str);
        }
        pszFmt++;
        continue;
      }

      /* Boolean */
      else if(*pszFmt == 'b'){
        bool bVal = (bool) va_arg(args, int);
        if(bVal){
            uart->print("True");
        } else {
            uart->print("False");
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
          error |= uart->write(ascii);
        }
        pszFmt++;
        continue;
      }
      /* Character */
      else if(*pszFmt == 'c') {
        uint8_t cVal = (uint8_t) va_arg(args, int);
        error |= uart->write(cVal);
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
          error |= uart->write(ascii);
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
          error |= uart->write(ascii);
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
          uart->print("NaN");
          pszFmt++;
          continue;
        }
        else if (val < -DBL_MAX){
          uart->print("fni-");
          pszFmt++;
          continue;
        }
        else if (val > DBL_MAX) {
          uart->print("fni+");
          pszFmt++;
          continue;
        }
        /* Test for large numbers */
        if( (val > UART_MAX_FLOAT) || (val < -UART_MAX_FLOAT) ) {
          uart->print("*reqEXP*");
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
        writeArray_rev(uart, buf, len);

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
* Function Name: printHeaderDT()
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

*
* \return
* The error code of the operation
*******************************************************************************/
uint32_t printHeaderDT(COMMS_UART_S* uart, const char* name, const char *date, const char* time){
  uint32_t error = COMMS_ERROR_NONE;
  printLn(uart, CLEAR_SCREEN_CMD);
  printLn(uart, "**************************************");
  error |= print(uart, "* %s\r\n*\r\n* Compiled at: %s on %s", name, time, date);
  error |= print(uart, "\r\n**************************************\r\n");
  return error;
}

/*******************************************************************************
* Function Name: printHeaderDTB()
****************************************************************************//**
*
* \brief Displays the program header with Date and time and build options. Use the macro 
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

*
* \return
* The error code of the operation
*******************************************************************************/
uint32_t printHeaderDTB(COMMS_UART_S* uart, const char* name, const char *date, const char* time){
  uint32_t error = COMMS_ERROR_NONE;
  printLn(uart, CLEAR_SCREEN_CMD);
  printLn(uart, "**************************************");
  error |= print(uart, "* %s\r\n", name);
  error |= print(uart, "*\r\n* Compiled at %s on %s", time, date);
#ifdef USE_BUILD_NUMBER
  error |= print(uart, "\r\n* Build %d.%d.%d", MajorRev, MinorRev, BuildNumber);
  CyDelay(15);
#endif
  error |= print(uart, "\r\n**************************************\r\n");
  return error;
}

/*******************************************************************************
* Function Name: uart_compareReg()
****************************************************************************//**
*
* \brief Prints the results of a register comparison
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
*
* \return
* The error code of the operation
*******************************************************************************/
uint32_t uart_compareReg(COMMS_UART_S* uart, const char* name, uint16_t actual, uint16_t expected){
  return print(uart, "%s: Received: 0x%x; Expected: 0x%x\r\n", name, actual, expected);
}

/*******************************************************************************
* Function Name: printErrorStatus()
****************************************************************************//**
*
* \brief Prints the results of a test
* 
* \param name [in]
*    Name of the program to display 
*
* \param error [in]
*    Possible error 
*
* \param subError [in]
*    Sub error code 
*
*
* \return
* The error code of the operation
*******************************************************************************/
uint32_t printErrorStatus(COMMS_UART_S* uart, const char* name, uint32_t error, uint32_t subError) {
  uint32_t e = COMMS_ERROR_NONE;
  if(error){
    e |= print(uart, "Error: %s - 0x%x suberror: 0x%x\r\n", name, error, subError);
  } else {
    e |= print(uart, "Success: %s\r\n", name);
  }
  return e;
}

/*******************************************************************************
* Function Name: writeArray_rev()
****************************************************************************//**
*
* \brief Prints an array in reverse order
* 
* \param name [in]
*    Name of the program to display 
*
* \param error [in]
*    Possible error 
*
* \param subError [in]
*    Sub error code 
*
*
* \return
* The error code of the operation
*******************************************************************************/
uint32_t writeArray_rev(COMMS_UART_S* uart, uint8_t *array, uint16_t len) {
  uint16_t i; 
  for(i=len-1; i>0; i--){
    uart->write(array[i]);
  }
  return uart->write(array[0]);
}

/*******************************************************************************
* Function Name: parseAsciiFloat()
****************************************************************************//**
* \brief Parses and ascii array and places the result into the result float
* 
* \param buf [in]
*    Buffer containing the input buffer 
*
* \param len [in]
*    Possible error 
*
* \param result [out]
*    Pointer to the float to place the result
*
* \return
* The error code of the operation
*******************************************************************************/
uint32_t parseAsciiFloat(uint8_t* buf, uint16_t len, float* result){
  uint32_t error = COMMS_ERROR_NONE;
  float output = 0.0;
  uint16_t i=0, decimalIdx = 0;
  bool decimal = false;
  bool negative = false;
  /* Check for negative */
  if(buf[0] == '-'){
    /* Replace with a zero for parsing */
    buf[0] = '0';
    negative = true;
  }
  /* Validate characters and find the decimal point */
  for(i=0; i<len; i++){
    uint8_t data = buf[i];
    /* Valid digit, do nothing */
    if(data >= '0' && data <= '9'){}
    /* Indicate that a decimal was found */
    else if(data == '.'){
      /* A decimal was already found */
      if(decimal){
        error = COMMS_ERROR_VAL;
        *result = i;
        break;
      }
      decimalIdx = i;
      decimal = true;
    } 
    /* Illegal character */
    else {
      error = COMMS_ERROR_VAL;
      /* Offending character position */
      *result = i;
      break;
    }
  }
  /* Valid data, continue with parsing */
  if(!error){
    /* A decimal was found */
    if(decimal) {
      /* Integer */
      for(i=0; i<decimalIdx; i++){
        /* Get value of digit */
        uint8_t val = buf[(decimalIdx-1)-i] - '0';
        output += val * pow10[i];
      }
      /* Fractional */
      for(i=0; i<len-decimalIdx-1; i++){
        /* Get value of digit */
        uint8_t val = buf[i + decimalIdx + 1] - '0';
        output += val / pow10[i+1];
      }
    }
    /* No decimal */
    else {
      for(i=0; i<len; i++){
        /* Get value of digit */
        uint8_t val = buf[(len-1)-i] - '0';
        output += val * pow10[i];
      }
    }
    /* Account for negative */
    if(negative){
      output = output * -1.0;
      /* Replace negative sign */
      buf[0] = '-';
    }
    /* Place output in array */
    *result = output;
  }


  return error;
}


/*******************************************************************************
* Function Name: getInputFloat()
****************************************************************************//**
* \brief Blocking function to get an input float via the UART
* 
* \param uart [in]
*   Pointer to the UART   
*
* \param result [out]
*    Pointer to the float to place the result
*
* \return
* The error code of the operation
*******************************************************************************/
uint32_t getInputFloat(COMMS_UART_S* uart, float* result) {
    uint8_t avail=0, input=0, inputBuffer[10]= {0};
    uint32_t error = COMMS_ERROR_NONE;
    print(uart, "Input: ");
    bool inputComplete = false;
    /* Reset the index */
    uint8_t idx = 0;
    while(!inputComplete) {
        /* Get new input */
        uart->getRxBufferSize(&avail);
        input=0;
        if(avail){
            uart->read(&input);   
            /* Complete */
            if(input == '\r') {
                /* Get rid of the newline character */
                uint8_t dummy;
                uart->read(&dummy);
                /* Parse the input */
                inputComplete = true;
                float f;
                error = parseAsciiFloat(inputBuffer, idx, &f);
                if(!error) {
                    *result = f;
                    printLn(uart, "");
                } else {
                    uint8_t errorIdx = (uint8_t) *result;
                    printLn(uart, "\r\nInvalid character at Pos: %i, \'%c\'\n", errorIdx, inputBuffer[errorIdx]);   
                }
            } 
            /* Echo and store */
            else {
                uart->write(input);
                inputBuffer[idx++] = input;                           
            }
        }  
    }
    return error;
}


/* [] END OF FILE */
