#include "controls.h"
#include "button.h"
#include "timer_updates.h"

extern int display_val;
char ohm_string[20];
int num;
int left;

int voltage_range = 3;
/** Voltage Range Notes
 * 3 -> 15 Volts (default)
 * 2 -> 5 Volts
 * 1 -> 1 Volts
 */

int current_range = 2;
/** Current Range Notes
 * 2 -> 200 mA (default)
 * 1 -> 10  mA
 */

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

/** conversions for the voltage mode **/
float update_voltage(int volt_range, float voltage) {
    float converted_val;
    switch(volt_range) {
        case 1:
            converted_val = (voltage * -0.724) + 1.191;
            break;
        case 2:
            converted_val = (voltage * -3.656) + 6.032;
            break;
        case 3:
            converted_val = (voltage * -9.6681) + 15.913;
            break;
    }
    return converted_val;
}

/** conversions for the current mode **/
float update_current(int current_range, float voltage) {
    float converted_curr;
    switch(current_range) {
       case 1:
           //have to add later
           break;
       case 2:
           converted_curr = (voltage * -3.41) + 5.628;
           break;
    }
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

/** get the raw voltage from the number **/
float get_voltage(int final) {
    float voltage = final / 65535.00;
    voltage = voltage * 3.3;
    voltage = update_voltage(voltage_range, voltage);
    auto_range_voltage(voltage);
    char volt_str[40];
    sprintf(volt_str, "mode %d | %.2f", voltage_range, voltage);
    UARTprintf("FINAL %s\n", volt_str);
    return voltage;
}

void auto_range_voltage(float voltage) {

    // 1V RANGE
    if ((voltage_range == 1) && (voltage > 0.9)) {
        voltage_range = 2;
    }

    // 5 VOLTS
    if ((voltage_range == 2) && (voltage < 0.9)) {
        voltage_range = 1;
    }

    if ((voltage_range == 2) && (voltage > 4.75)) {
        voltage_range = 3;
    }

    // 15 VOLT RANGE
    if ((voltage_range == 3) && (voltage < 4.75)) {
        voltage_range = 2;
    }
    // update frontend
    switch(voltage_range) {
        // 1V range
        case 1:
            set_frontend_state(0b11000010);
            break;

       // 5V range
        case 2:
            set_frontend_state(0b11000100);
            break;

        // 15V range
        case 3:
            set_frontend_state(0b11000000);
            break;
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
