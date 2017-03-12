# multimeter-firmware

> The firmware for the multimeter

## Supported Devices

The Tiva TM4C123G is the supported device for this project

## Building

1. Install Code Composer Studio
2. Download the [TivaWare Software](https://focus-webapps.ti.com/licreg/docs/swlicexportcontrol.tsp?form_type=2&prod_no=SW-TM4C-2.1.4.178.exe&ref_url=http://software-dl.ti.com/tiva-c/SW-TM4C/latest/&form_id=154910)
3. Import this project into Code Composer Studio
4. Add the TivaWare libraries by:
    - Go to Project > Properties
    - Under Build > ARM Compiler > Include Options, add the TivaWare folder to the '#include search path'
    - Under Build > ARM Linker > File Search Path, include the library file located at $TIVA-FOLDER/driverlib/ccs/Debug/driverlib.lib and add the folder $TIVA-FOLDER/inc to the library search path
5. Build the program and test program the board


