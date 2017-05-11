#include "controls.h"
#include "button.h"
extern int display_val;
char ohm_string[20];
int num;
int left;

/** get the raw voltage from the number **/
float get_voltage() {
    // convert this to a raw voltage
    long long final = (display_val);
    float voltage = (float)final/65536.00;
    voltage = voltage * 3.3;
    long long vol = (voltage * 1000);
    int num1 = vol / 1000;
    int left1 = vol - (num * 1000);
    UARTprintf("\r   vol %d.%d \r",num1, left1);
    return voltage;
}

/** convert the raw reading for 1k range **/
void convert_ohm_1k(float voltage) {
    float ohm = (voltage * -659) + 1101;
    long long vol = (ohm * 1000);
    num = vol / 1000;
    left = vol - (num * 1000);
    UARTprintf("\r ohm  %d.%d \r",num, left);
}

/** convert the raw reading for 1M range **/
void convert_ohm_1M(float voltage) {
   float ohm = (voltage * -653) + 1096;
   long long ohm_c = (ohm * 1000);
   num = ohm_c / 1000;
   left = ohm_c - (num * 1000);
   UARTprintf("\r ohm  %d.%d \r", num, left);
}

// comes into the function after the adc read
void update_hardware() {
    if (my_mode == OHMETER) {
        UARTprintf("Ohm meter\n");
        float volt = get_voltage();
        convert_ohm_1k(volt);
        // should be able to auto range here
    }
    // get the ADC read value
    // convert to OHM
}


/** control to be interfaced with the hardware part of the circuit**/
void init_hardware_control() {
    // Initialize the OHM meter control (port B, pin 5)
    SysCtlPeripheralEnable(OHM_PERI);
    GPIOPinTypeGPIOOutput(OHM_PORT,  OHM_PIN_0);
    GPIOPinWrite(OHM_PORT, OHM_PIN_0 , 0);
}
