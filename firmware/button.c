#include "button.h"
#include "mux.h"
#include "timer_updates.h"  q
void check_buttons() {
    //UARTprintf("checking button\n");
    if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0) == GPIO_PIN_0) {
        UARTprintf("Up pressed\n");
        sample_index++;
    }
    if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1) == GPIO_PIN_1) {
        UARTprintf("Down pressed\n");
        sample_index--;
    }

    // loop back the index
    if (sample_index < 0) {
        sample_index = 0;
    } else if (sample_index > 4) {
        sample_index = 4;
    }
}

void init_buttons() {
    // enable the port E
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    // set pin 0,1,2,3  as input
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_2);
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_3);
    // check if they are pressed
}
