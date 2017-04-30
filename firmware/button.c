#include "button.h"
#include "mux.h"

extern int mux_state;
void update_mux_state() {
    if (mux_state == 4) {
        mux_state = 1;
    }
    UARTprintf("Mux state %d\n", mux_state);
    output_state();
}

int button_count = 0;
void button_handler() {
    uint32_t status = 0;
    status = GPIOIntStatus(ButtonBase, true);
    GPIOIntClear(ButtonBase, status);
    if(status & ButtonInt == ButtonInt) {
        button_count++;
        UARTprintf("Button Interrupt %d\n", button_count);
    }
}

void init_interrupt_button() {
    GPIOPinTypeGPIOInput(ButtonBase, Button);
    GPIOPadConfigSet(ButtonBase , Button, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_RISING_EDGE);
    GPIOIntRegister(GPIO_PORTE_BASE, button_handler);
    GPIOIntEnable(GPIO_PORTE_BASE, GPIO_INT_PIN_0);
}
