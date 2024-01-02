# Majestic Labs Embedded Drivers
Middleware for embedded system 
* Cross Platform


# Usage

## Building the library
Use WSL to build 

`make clean`
`make all` 

## Configuration
### PSoC Creator 
1. Add file locations to project
    * Project → Build Settings
    * `Project Name` → `Compiler Name` → Complier → General
    * Add the mjl_driver directory to `Additional Include Directories`
        * Add `include`
        * Add `src` if building from source 
2. Add files to Creator project
    * In the workspace explorer, right click `Header Files` → Add → Existing Items
    * Add the relevant header files
    * Add the relevant source files, if building from source
3. Add the pre-compiled library if NOT building from source 
    * Project → Build Settings
    * `Project Name` → `Compiler Name` → Linker → General
    * Add `mjl_drivers` to `Additional libraries`
        * NOTE: do _not_ include `'lib'` or `'.a'`
    * Add the library location to `Additional Library Directories`
        * It is not required to include `.` if the library is in the source directory


## Hardware Abstraction Layer (HAL)
1. Create a new file from a HAL template
2. Configure the HAL file to match your specific settings

## Driver Configuration 
1. Pass in functions to the configuration structure from the HAL
```C
/* Start the UART */
MJL_UART_CFG_S uartCfg = uart_cfg_default;
uartCfg.hal_req_writeArray = uart_psoc4SCB_writeArrayBlocking;
uartCfg.hal_req_read = uart_psoc4SCB_read;
uartCfg.hal_opt_externalStart = uart_psoc4SCB_start;
uartCfg.hal_opt_externalStop = uart_psoc4SCB_stop;
error |= uart_init(&usb, &uartCfg);
error |= uart_start(&usb);
```