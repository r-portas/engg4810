#include "button.h"
#include "mux.h"
#include "timer_updates.h"
#include "adc.h"

char *message[] = {"Voltmeter >> ", "Ampmeter >> " , "Ohmeter >> ", "Continuity >>", "Logic >>", "AC >>"};
char *msgUpdate [] = {"Voltmeter AC|DC ", "Ampmeter AC | DC" , "Mode:Ohmmeter" , "Mode: Continuity", "Mode: Logic"};

int msg_count = 0;
int my_mode = 0;
int my_state    = NONE;
extern int ac_set;

void count_check() {
    if (msg_count < 0) {
        msg_count = 0;
    } else if (msg_count > 4) {
        msg_count = 4;
    }
    if (sample_index < 0) {
        sample_index = 0;
    } else if (sample_index > 4) {
        sample_index = 4;
    }
}

// update the internal state for the tiva
void update_mode() {
    switch(my_state) {
        case NONE:
            my_state = STATE_SELECTION;
            my_mode = (msg_count + 12);
            break;
        case STATE_SELECTION:
            my_state = STATE_MEASURE;
            break;
        case STATE_MEASURE:
            // going into sd and ask samples
            if (sd_state == 1) {
                sd_state = 0;
            } else if (sample_count > 0) {
                my_state = ASK_SAMPLES;
            }
            break;
        case ASK_SAMPLES:
            my_state = STATE_MEASURE;
            sd_state = 1;
            break;
    }
}

void update_sd_count() {
    // loop
    if (sd_sample_index < 0) {
        sd_sample_index = 0;
    } else  if (sd_sample_index > 8) {
        sd_sample_index = 8;
    }
    // update the count
    sd_samples_ask = sample_list[sd_sample_index];
    UARTprintf("samples asked %d  | %d | %d \n", sd_samples_ask, ask_samples[sd_sample_index], sd_sample_index);
}

void check_buttons() {
    /** Respond to the Up button ***/
    if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0) == GPIO_PIN_0) {
        UARTprintf("Up pressed\n");
        if (my_state == STATE_MEASURE) {
            sample_index++;
            count_check();
            // check for memory error
        }
        /** select AC **/
        if (my_state == STATE_SELECTION) {
            ac_set = 1;
        }

        if (my_state == NONE) {
            // increment message count
            msg_count++;
            count_check();
        }
        if (my_state == ASK_SAMPLES) {
            sd_sample_index++;
            update_sd_count();
        }
    }

    /** Respond to the Down button ***/
    if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1) == GPIO_PIN_1) {
        UARTprintf("Down pressed\n");
        if (my_state == STATE_MEASURE) {
            //msg_count--;
            UARTprintf("state measure\n");
            //msg_count_check();
            sample_index--;
            count_check();
            UARTprintf("sample index %d", sample_index);
            // check for memory error
        }
        if (my_state == NONE) {
            // increment msg count
            msg_count--;
            count_check();
        }
        /** go to DC mode **/
        if (my_state == STATE_SELECTION) {
            ac_set = 0;
        }
        if (my_state == ASK_SAMPLES) {
            sd_sample_index--;
            update_sd_count();
        }
    }

    /** Respond to the Enter button ***/
    if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2) == GPIO_PIN_2) {
        UARTprintf("Enter pressed\n");
        update_mode();
     }

    /** Response to the back button **/
    if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3) == GPIO_PIN_3) {
        UARTprintf("Back pressed\n");
        msg_count = 0;
        my_mode = 0;
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
