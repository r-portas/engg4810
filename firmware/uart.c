#include "uart.h"

void init_uart() {
    if (EXTERNAL_UART) {
        //
        // Enable the GPIO Peripheral used by the UART.
        //
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

        //
        // Enable UART1
        //
        SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);

        //
        // Configure GPIO Pins for UART mode.
        //
        GPIOPinConfigure(GPIO_PB0_U1RX);
        GPIOPinConfigure(GPIO_PB1_U1TX);
        GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

        //
        // Use the internal 16MHz oscillator as the UART clock source.
        //
        UARTClockSourceSet(UART1_BASE, UART_CLOCK_PIOSC);

        //
        // Initialize the UART for console I/O.
        //
        UARTStdioConfig(1, 9600, 16000000);

    } else {
        //
        // Enable the GPIO Peripheral used by the UART.
        //
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

        //
        // Enable UART0
        //
        SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

        //
        // Configure GPIO Pins for UART mode.
        //
        GPIOPinConfigure(GPIO_PA0_U0RX);
        GPIOPinConfigure(GPIO_PA1_U0TX);
        GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

        //
        // Use the internal 16MHz oscillator as the UART clock source.
        //
        UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

        //
        // Initialize the UART for console I/O.
        //
        UARTStdioConfig(0, 9600, 16000000);
    }
}
