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
#include "controls.h"
#include "timer_updates.h"

void hardware_init() {
   init_LCD();
   init_uart();
   init_hardware_control();
   init_buttons();
   init_sd_card();
   roy_adc();
   init_timers();
}
// one reading per clock tick// call ADC read in a timer
int main() {
    // set the clock frequency and the clock divider
    // SYSCTL_SYSDIV_2_5 < 80mz clock
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                         SYSCTL_XTAL_16MHZ);
    hardware_init();

    float reading = 0.5;
    char mode = 'V';

    while(1) {
        UARTprintf("hello\n");
        // do nothing
        UARTprintf(".\n");
        write_log_line(reading, mode);
        reading += 0.5;

        //buttonInterrupt();
        //write_file();
        SysCtlDelay(100000);
    }
}
