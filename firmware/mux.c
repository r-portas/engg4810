#include "mux.h"
int mux_state = 3;
void init_mux() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinTypeGPIOOutput(MUX_PORT_BASE , A);
    GPIOPinTypeGPIOOutput(MUX_PORT_BASE , B);
    GPIOPinTypeGPIOOutput(MUX_PORT_BASE , C);

    // Initial state
    GPIOPinWrite(MUX_PORT_BASE, A , 0);
    GPIOPinWrite(MUX_PORT_BASE, B , 0);
    GPIOPinWrite(MUX_PORT_BASE, C , C);
}


void output_state() {
    UARTprintf("updating mux - ");
    switch(mux_state) {
        case 1:
            UARTprintf("%d\n", mux_state);
            GPIOPinWrite(MUX_PORT_BASE, A , 0);
            GPIOPinWrite(MUX_PORT_BASE, B , 0);
            GPIOPinWrite(MUX_PORT_BASE, C , 0);
            break;

        case 2:
            UARTprintf("%d\n", mux_state);
            GPIOPinWrite(MUX_PORT_BASE, A , 0);
            GPIOPinWrite(MUX_PORT_BASE, B , B);
            GPIOPinWrite(MUX_PORT_BASE, C , 0);
            break;

        case 3:
            UARTprintf("%d\n", mux_state);
            GPIOPinWrite(MUX_PORT_BASE, A , 0);
            GPIOPinWrite(MUX_PORT_BASE, B , 0);
            GPIOPinWrite(MUX_PORT_BASE, C , C);
            break;
    }
}
