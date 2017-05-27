#include "controls.h"
#include "button.h"
#include "timer_updates.h"

extern int display_val;
char ohm_string[20];
int num;
int left;

void set_frontend_state(uint8_t val) {
    GPIOPinWrite(GPIO_PORTC_BASE, SHCP , 0);
    GPIOPinWrite(GPIO_PORTC_BASE, STCP , 0);

    uint8_t new_val = val << 4;
    new_val = new_val & 0b00010000;
    GPIOPinWrite(GPIO_PORTC_BASE, DS, new_val);

    GPIOPinWrite(GPIO_PORTC_BASE, SHCP , SHCP);
    SysCtlDelay(100);
    GPIOPinWrite(GPIO_PORTC_BASE, SHCP , 0);


    new_val = val << 3;
    new_val = new_val & 0b00010000;
    GPIOPinWrite(GPIO_PORTC_BASE, DS, new_val);

    GPIOPinWrite(GPIO_PORTC_BASE, SHCP , SHCP);
    SysCtlDelay(100);
    GPIOPinWrite(GPIO_PORTC_BASE, SHCP , 0);


    new_val = val << 2;
    new_val = new_val & 0b00010000;
    GPIOPinWrite(GPIO_PORTC_BASE, DS, new_val);

    GPIOPinWrite(GPIO_PORTC_BASE, SHCP , SHCP);
    SysCtlDelay(100);
    GPIOPinWrite(GPIO_PORTC_BASE, SHCP , 0);


    new_val = val << 1;
    new_val = new_val & 0b00010000;
    GPIOPinWrite(GPIO_PORTC_BASE, DS, new_val);

    GPIOPinWrite(GPIO_PORTC_BASE, SHCP , SHCP);
    SysCtlDelay(100);
    GPIOPinWrite(GPIO_PORTC_BASE, SHCP , 0);

    new_val = val << 0;
    new_val = new_val & 0b00010000;
    GPIOPinWrite(GPIO_PORTC_BASE, DS, new_val);

    GPIOPinWrite(GPIO_PORTC_BASE, SHCP , SHCP);
    SysCtlDelay(100);
    GPIOPinWrite(GPIO_PORTC_BASE, SHCP , 0);


    // Other direction

    new_val = val >> 1;
    new_val = new_val & 0b00010000;
    GPIOPinWrite(GPIO_PORTC_BASE, DS, new_val);

    GPIOPinWrite(GPIO_PORTC_BASE, SHCP , SHCP);
    SysCtlDelay(100);
    GPIOPinWrite(GPIO_PORTC_BASE, SHCP , 0);


    new_val = val >> 2;
    new_val = new_val & 0b00010000;
    GPIOPinWrite(GPIO_PORTC_BASE, DS, new_val);

    GPIOPinWrite(GPIO_PORTC_BASE, SHCP , SHCP);
    SysCtlDelay(100);
    GPIOPinWrite(GPIO_PORTC_BASE, SHCP , 0);


    new_val = val >> 3;
    new_val = new_val & 0b00010000;
    GPIOPinWrite(GPIO_PORTC_BASE, DS, new_val);

    GPIOPinWrite(GPIO_PORTC_BASE, SHCP , SHCP);
    SysCtlDelay(100);
    GPIOPinWrite(GPIO_PORTC_BASE, SHCP , 0);

    GPIOPinWrite(GPIO_PORTC_BASE, STCP , STCP);
}

float update_voltage(int volt_range, float voltage) {
    switch(volt_range){
        case 1:
            voltage = (voltage * -0.5897) + 0.9534;
            break;
        case 2:
            voltage = (voltage * -2.9462) + 4.8103;
            break;
        case 3:
            voltage = (voltage * -9.6681) + 15.913;
    }
    return voltage;
}

/** convert the raw reading for 1k range **/
float convert_ohm_1k(float voltage) {
    float ohm = (voltage * -659) + 1101;
    long long vol = (ohm * 1000);
    num = vol / 1000;
    left = vol - (num * 1000);
    return ohm;
}
/** convert the raw reading for 1M range **/
float convert_ohm_1M(float voltage) {
   float ohm = (voltage * -653) + 1096;
   long long ohm_c = (ohm * 1000);
   num = ohm_c / 1000;
   left = ohm_c - (num * 1000);
   return ohm;
}

/** 3 -> 15 volts **/
int voltage_range = 3;
/** get the raw voltage from the number **/
float get_voltage(int final) {
    // convert this to a raw voltage
    //long long final = (display_val);
    float voltage = final / 65536.00;
    voltage = voltage * 3.3;

    long long vol = (voltage * 1000);
    int num1 = vol / 1000;
    int left1 = vol - (num * 1000);

    UARTprintf("DEBUG %d.%d\n", num1, left1);
    voltage = convert_ohm_1M(voltage);
    vol = (voltage * 1000);
    num1 = vol / 1000;
    left1 = vol - (num * 1000);
    UARTprintf("FINAL %d.%d\n", num1, left1);

    return voltage;
}




void auto_range_voltage(float voltage) {
    // switch between the states
    // 1V RANGE
    if ((voltage_range == 1) && (voltage > 5)) {
        voltage_range = 3;
    }

    if ((voltage_range == 1) && (voltage > 0.95)) {
        voltage_range = 2;
        set_frontend_state(0b11000010 );
    }

    // 5 VOLTS
    if ((voltage_range == 2) && (voltage < 0.9)) {
        voltage_range = 1;
        set_frontend_state( 0b11000100 );
    }

    if ((voltage_range == 2) && (voltage > 5.5)) {
        voltage_range = 3;
        // set fronend 15 volts
    }

    // 15 VOLT RANGE
    if ((voltage_range == 3) && (voltage < 5)) {
        voltage_range = 2;
    }
}

void update_fronend() {
    int mapped_volts;
    int raw_val = display_val;
    int raw_voltage = get_voltage(display_val);
    // 15 Volts default
    if (my_mode == VOLTMETER) {
        mapped_volts = update_voltage(3, raw_voltage);
        auto_range_voltage(mapped_volts);
    }
}

/*float update_current(int curr_range, float voltage) {
    float value;
    switch(curr_range) {
        case 1:
            value =
            break;
        case 2:
            value =
            break;
        case 3:
            value =
            break;
    }
    return value;
}*/

float convert_logic(float voltage) {
   float ohm = (voltage * -6.7642) + 9.8786;
   long long log_c = (ohm * 1000);
   num = log_c / 1000;
   left = log_c - (num * 1000);
   return ohm;
}

// comes into the function after the adc read
void update_hardware() {
    if (my_mode == OHMETER) {
        float volt = get_voltage(display_val);
        convert_ohm_1k(volt);
        // should be able to auto range here
    }
    // for the continuity
    if (my_mode == LOGIC) {
        float volt = get_voltage(display_val);
        float ohm = convert_logic(volt);
        if (ohm < 5.00) {
            buzzer_state = 1;
        } else {
            buzzer_state = 0;
        }
    }
}

/** control to be interfaced with the hardware part of the circuit**/
void init_hardware_control() {
    // Initialize the OHM meter control (port B, pin 2)
    SysCtlPeripheralEnable(OHM_PERI);
    GPIOPinTypeGPIOOutput(OHM_PORT,  OHM_PIN_0);
    GPIOPinWrite(OHM_PORT, OHM_PIN_0 , 0);

    // Initialize the OHM meter control (port B, pin 3)
    // Initialize the Continuity control pin
    SysCtlPeripheralEnable(CONT_PERIPH);
    GPIOPinTypeGPIOOutput(CONTI_PORT,  CONT_PIN);
    GPIOPinWrite(CONTI_PORT, CONT_PIN , 0);
}
