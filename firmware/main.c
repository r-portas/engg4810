#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h" 
#include "driverlib/sysctl.h"
#include "driverlib/rom_map.h"
#include "utils/uartstdio.h"
#include "lcd.h"
#include "led.h"
#include "uart.h"


// State machine states
enum state_t {
    LCD,
    LED,
    SD_CARD,
    UART
} state = LCD;


void state_machine() {
    switch (state) {
        case LCD:
            ledOn(RED_LED);
            state = LED;
            break;

        case LED:
            ledOn(BLUE_LED);
            state = SD_CARD;
            break;

        case SD_CARD:
            ledOn(GREEN_LED);
            state = UART;
            break;

        case UART:
            UARTprintf("UART PRINTING STUFF");
            state = LCD;
            break;
    }
}

int main() {
    initLCD();
    initLed();
    initUart();
    storeSpecialChar();

    while (1) {
        state_machine();
        // clearLCD();
        // sendSpecialChar();
        // sending the position of
        // sendByte(0x00, TRUE);
        /* printLCD("Hello World !!"); */
        Delay(10000000);
    }
}
