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
            my_mode = LOGIC;
            break;
        case 'v':
            my_mode = VOLTMETER;
            break;
        case 'c':
            my_mode = AMPMETER;
            break;
        case 'r':
            my_mode = OHMETER;
            break;
        case 's':

            // update the sampling mode
            break;
        default:
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

/** r c v **/
static char get_mode_char() {
    if (my_mode == VOLTMETER) {
        return 'v';
    } else if (my_mode == AMPMETER) {
        return 'c';
    } else if (my_mode == OHMETER) {
        return 'r';
    } else {
        return '!';
    }
}

void send_mode() {
    char mode_send = get_mode_char();

    UARTprintf("%c\n", mode_send);
}

/** Formatting for pc communication **/
void send_pc() {
    UARTprintf("# %d %d \n", data_buff[sample_count], sample_count);
    // mode followed by the sampling rate
    if (my_state == STATE_MEASURE) {
        if (ac_set) {
            UARTprintf("$ &#177; AC %d.%d %c <br> %s\n", num1, left1, char_my_mode, sample_msg[sample_index]);
        } else {
            UARTprintf("$ &#177; DC %d.%d %c <br> %s\n", num1, left1, char_my_mode, sample_msg[sample_index]);
        }
    } else if (my_state == NONE) {
        UARTprintf("$ %s\n", message[msg_count]);
    } else if (my_state == STATE_SELECTION) {
        UARTprintf("$ %s\n", msgUpdate[msg_count]);
    }
}
