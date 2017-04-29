#include "state_machine.h"

// State machine states
enum state_t {
    LCD,
    LED,
    SD_CARD,
    UART,
    ADC
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
            state = ADC;
            break;

        case ADC:
            state = LCD;
            break;
    }
}
