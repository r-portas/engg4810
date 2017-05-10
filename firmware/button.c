#include "button.h"
#include "mux.h"
#include "timer_updates.h"

char *message[] = {"Voltmeter -> ", "Ampmeter -> " , "Ohmeter -> ", "Logic ->"};
char *msgUpdate [] = {"Mode:Voltmeter", "Mode:Ampmeter" , "Mode:Ohmmeter" , "Mode:Logic"};

int msg_count = 0;
int my_mode = 0;
int my_state    = NONE;

void msg_count_check() {
    if (msg_count < 0) {
        msg_count = 0;
    } else if (msg_count > 3) {
        msg_count = 3;
    }
}


void check_buttons() {
    /** Respond to the Up button ***/
    if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0) == GPIO_PIN_0) {
        UARTprintf("Up pressed\n");
        if (my_state == STATE_MEASURE) {
            sample_index++;
            // check for memory error
        }
        if (my_state == NONE) {
            // increment message count
            msg_count++;
            msg_count_check();
        }
    }
    /** Respond to the Down button ***/
    if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1) == GPIO_PIN_1) {
        UARTprintf("Down pressed\n");
        if (my_state == STATE_MEASURE) {
            //msg_count--;
            //msg_count_check();
            sample_index--;
            // check for memory error
        }
        if (my_state == NONE) {
            // increment msg count
            msg_count--;
            msg_count_check();

        }
    }

    /** Respond to the Enter button ***/
    if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2) == GPIO_PIN_2) {
        UARTprintf("Enter pressed\n");
        if (my_state == NONE) {
            // start reading the data
            my_state = STATE_SELECTION;
            my_mode = (msg_count + 12);
            //msg_count = 0;  // reset
        }
        if (my_state == STATE_SELECTION) {
            my_state = STATE_MEASURE;
        }
     }

    /** Response to the back button **/
    if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3) == GPIO_PIN_3) {
        UARTprintf("Back pressed\n");
        msg_count = 0;
        my_state = NONE;    // go back regardless of the state
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
