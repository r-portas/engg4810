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

char input[30];
int inputIndex = 0;

void process_command() {
    switch(input[0]) {
        case 'l':
            // Change to logic level
            // UARTprintf("LOGIC LEVEL VOLTAGE\n");
            my_state = LOGIC;
            break;
        case 'v':
            // Change to voltage mode
            // UARTprintf("MULTIMETER VOLTAGE\n");
            my_state = VOLTMETER;
            break;
        case 'c':
            // Change to current mode
            // UARTprintf("MULTIMETER CURRENT\n");
            my_state = AMPMETER;
            break;
        case 'r':
            // Change to current mode
            // UARTprintf("MULTIMETER RESISTANCE\n");
            my_state = OHMETER;
            break;
        case 's':
            // UARTprintf("SD Card log\n");
            break;
        default:
            // Probably an error
            break;
    }
}

void read_uart() {
    int32_t inputChar;

    if (EXTERNAL_UART) {
        if (UARTCharsAvail(UART1_BASE) == false) {
            return;
        }
        inputChar = UARTCharGetNonBlocking(UART1_BASE);
    } else {
        if (UARTCharsAvail(UART0_BASE) == false) {
            return;
        }
        inputChar = UARTCharGetNonBlocking(UART0_BASE);
    }

    if (inputChar != -1) {
        // We have a valid character
        char validChar = (char)inputChar;
        input[inputIndex] = validChar;
        inputIndex++;

        // Check if we have a end of line
        if (validChar == '\n') {
            // Add EOL
            input[inputIndex+1] = '\0';
            // Process string
            // UARTprintf(input);
            process_command();
            inputIndex = 0;
        }
    }
    
    if (inputIndex >= 30) {
        // Reset, in case of error
        inputIndex = 0;
    }
}
