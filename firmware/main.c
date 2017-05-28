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
#include "inc/hw_gpio.h"
#include "utils/uartstdio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/fpu.h"
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

int sd_flag = 0;

void hardware_init() {
   FPUEnable();
   FPULazyStackingEnable();
   init_LCD();
   init_uart();
   init_hardware_control();
   init_buttons();
   //init_sd_card();
   roy_adc();
   init_backlight();
   init_timers();
}

void setup_frontend() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlDelay(3);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,  DS);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,  STCP);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,  SHCP);
}

int main() {
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                         SYSCTL_XTAL_16MHZ);
    hardware_init();
    // Rail load switch high
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,  GPIO_PIN_5);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5 , GPIO_PIN_5);

    // LEDs
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE,  GPIO_PIN_6);
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6 , GPIO_PIN_6);
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE,  GPIO_PIN_7);
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7 , GPIO_PIN_7);

    setup_frontend();
    GPIOPinWrite(GPIO_PORTC_BASE, DS , DS);
    GPIOPinWrite(GPIO_PORTC_BASE, SHCP , SHCP);
    GPIOPinWrite(GPIO_PORTC_BASE, STCP , STCP);

    // Voltage measure, 12v mode
    set_frontend_state(0b11000000);

    // Voltage measure, 5v mode
    //set_frontend_state( 0b11000100 );

    // Voltage measure, 1v mode
    //set_frontend_state( 0b11000010);

    // Logic Probe
    // set_frontend_state(0b11100000);

    // Current measure, 200ma range
    //set_frontend_state( 0b11010110 );

    // Current measure, 10ma range
    //set_frontend_state( 0b11010001 );
    // Ohmmeter, 1k range
    //   set_frontend_state(0b00001101);

    // Ohmmeter, 1M range
    // set_frontend_state(0b10001011);

    // set_frontend_state( 0b00000000 );
    float reading = 0.5;
    char mode = 'V';
    //UARTprintf("Starting");
    while(1) {
        buttonInterrupt();
        read_uart();
    }
}
