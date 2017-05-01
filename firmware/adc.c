#include "adc.h"
#include "mux.h"
uint32_t DataRx[1000];
#define NUM_SSI_DATA            3
uint32_t pui32DataRx[NUM_SSI_DATA];
float voltage = 0.00;
int range = 2;
extern int mux_state;

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

void adc_read() {
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 , GPIO_PIN_3);
    SysCtlDelay(8);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 , 0);
    SysCtlDelay(20);
    SSIDataPut(SSI1_BASE, 'D');
    while(SSIBusy(SSI1_BASE))
    {

    }
    SSIDataGet(SSI1_BASE, &pui32DataRx[0]);
    //UARTprintf("data %d\n", pui32DataRx[0]);
    /*long long  final = (pui32DataRx[0] + 55);
    float voltage = (float)final/65536.00;
    voltage = voltage * 4.8;
    voltage = update_voltage(voltage);
    auto_range(voltage);
    long long vol = (voltage * 1000);
    int num = vol / 1000;
    int left = vol - (num * 1000);
    UARTprintf("\r    raw %d vol %d.%d RANGE %d   \r", pui32DataRx[0], num, left, range);
    */
}

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


void roy_adc() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    GPIOPinConfigure(GPIO_PF0_SSI1RX);
    GPIOPinConfigure(GPIO_PF1_SSI1TX);
    GPIOPinConfigure(GPIO_PF2_SSI1CLK);


    GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_SSI1)) {
    }

    SSIConfigSetExpClk(SSI1_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 1000000, 16);
    SSIEnable(SSI1_BASE);
}
