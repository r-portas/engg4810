////////// INCLUDES
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_ssi.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/uart.h"
#include "driverlib/rom_map.h"
#include "driverlib/ssi.h"
#include "utils/uartstdio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "utils/fatfs/src/ff.h"
#include "utils/fatfs/src/diskio.h"

#include "lcd.h"
#include "led.h"
#include "uart.h"
#include "sd_card.h"
#include "button.h"
#include "mux.h"
#include "adc.h"
#include "timer_updates.h"

void hardware_init() {
   initTimer();
   init_LCD();
   init_uart();
   init_sd_card();
   init_interrupt_button();
   init_mux();
   roy_adc();
}

// call adc read in a timer
int main() {
    // set the clock frequency
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                         SYSCTL_XTAL_16MHZ);
    // Initialize the hardware module
    // hardware_init();
    // don't play with the watch dog timer
    hardware_init();
    while(1) {
        adc_read();
        //storeSpecialChar();
        //sendSpecialChar();
        //sendByte(0x00, lcd_true);
        //printLCD("HELLO !!! ");
    }
}
