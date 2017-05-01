#include "button.h"
#include "mux.h"

void check_buttons() {
    //UARTprintf("checking button\n");
    if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0) == GPIO_PIN_0) {
        UARTprintf("Up pressed\n");
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
