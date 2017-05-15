#include "adc.h"
#include "mux.h"
#include "lcd.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"

float voltage = 0.00;
int range = 2;
extern int mux_state;
int display_val = 0;

float update_voltage(float voltage) {
    switch(range){
        case 1:
            voltage = (voltage * -0.5897) + 0.9534;
            break;
        case 2:
            voltage = (voltage * -2.9462) + 4.8103;
            break;
        case 3:
            voltage = (voltage * -7.0312) + 11.993;
    }
    return voltage;
}

void auto_range(float voltage) {
    // switch between the states
    // 1V RANGE
    if ((range == 1) && (voltage > 0.95)) {
        range = 2;
        GPIOPinWrite(MUX_PORT_BASE, B , 0);
        GPIOPinWrite(MUX_PORT_BASE, C , C);

    }
    // 5 VOLTS
    if ((range == 2) && (voltage < 0.9)) {
        range = 1;
        GPIOPinWrite(MUX_PORT_BASE, B , B);
        GPIOPinWrite(MUX_PORT_BASE, C , 0);
    }
}

long debug_count = 0;
uint32_t data_buff[2000];
int sample_count = 0;
uint32_t data_count = 0;

void adc_read() {
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 , GPIO_PIN_3);
    SysCtlDelay(6);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 , 0);
    SysCtlDelay(15);
    SSIDataPut(SSI1_BASE,  0xFFFF);
    while(SSIBusy(SSI1_BASE))
    {

    }
    SSIDataGet(SSI1_BASE, &data_buff[sample_count]);
    while(SSIBusy(SSI1_BASE))
    {

    }
    /** KEEP THE FORMAT FOR ROY **/
    /* UARTprintf("\n r %d %d \n", data_buff[sample_count], sample_count); */
    display_val =  data_buff[sample_count];
    sample_count++;
    // wrap around
    if (sample_count > 2000) {
        sample_count = 0;
    }
    sample_count++;
}
/*
void convert_display() {
      long long final = (pui32DataRx[0] + 55);
      float voltage = (float)final/65536.00;
      voltage = voltage * 4.8;
      // updates the voltage
      voltage = update_voltage(voltage);
      // auto range the voltage
      auto_range(voltage);
      long long vol = (voltage * 1000);
      int num = vol / 1000;
      int left = vol - (num * 1000);
      UARTprintf("\r    raw %d vol %d.%d RANGE %d   \r", pui32DataRx[0], num, left, range);
}*/

void init_adc() {
    // configure the clock for ADC
    // enable the SSI and GPIO port
    SysCtlPeripheralEnable(ADC_SSI_SYSCTL_PERIPH);
    while(!SysCtlPeripheralReady(ADC_SSI_SYSCTL_PERIPH)) {

    }
    //SSIClockSourceSet(SYSCTL_PERIPH_SSI0, SSI_CLOCK_SYSTEM);
    SysCtlPeripheralEnable(ADC_GPIO_SYSCTL_PERIPH);
    GPIOPinTypeSSI(ADC_GPIO_PORT_BASE, TX_PIN | RX_PIN | ADC_SCLK_PIN);
    // Configure the CS pin as output to select CS
    GPIOPinTypeGPIOOutput(ADC_GPIO_PORT_BASE, GPIO_PIN_3);
    // Configure the clock settings
    // SPI mode, master mode, 1MHz SSI frequency, and 8-bit data
    SSIConfigSetExpClk(ADC_SSI_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                           SSI_MODE_MASTER, 1000000, 16);

    // Link clock and enable SSI for conversion
    SSIEnable(ADC_SSI_BASE);
}


/** Unlock the pin 0 on port F **/
void unlock_portF_pins() {
    HWREG(GPIO_PORTF_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE+GPIO_O_CR)  |= GPIO_PIN_0;
}

void roy_adc() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    unlock_portF_pins();

    GPIOPinConfigure(GPIO_PF1_SSI1TX);
    GPIOPinConfigure(GPIO_PF2_SSI1CLK);
    GPIOPinConfigure(GPIO_PF0_SSI1RX);

    GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 );
    GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_0);

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_SSI1)) {

    }
    SSIConfigSetExpClk(SSI1_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 1000000, 16);
    SSIEnable(SSI1_BASE);
}
