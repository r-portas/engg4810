#include "interrupts.h"

void timerInterruptHandler() {
    if (GPIOIntStatus(INTERRUPT_PORT, false) & INTERRUPT_PINS) {
        // PF4 was interrupt cause
        UARTprintf("Button Down\n");

        // Clear interrupt flag
        GPIOIntClear(INTERRUPT_PORT, INTERRUPT_PINS);  
    }
}

void initInterrupts() {
    // Enable Port B
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPinTypeGPIOInput(INTERRUPT_PORT, INTERRUPT_PINS);
    GPIOPadConfigSet(INTERRUPT_PORT, INTERRUPT_PINS,
            GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOIntDisable(INTERRUPT_PORT, INTERRUPT_PINS);
    GPIOIntClear(INTERRUPT_PORT, INTERRUPT_PINS);
    GPIOIntRegister(INTERRUPT_PORT, timerInterruptHandler);
    GPIOIntTypeSet(INTERRUPT_PORT, INTERRUPT_PINS,
            GPIO_FALLING_EDGE);
    GPIOIntEnable(INTERRUPT_PORT, INTERRUPT_PINS);
}

