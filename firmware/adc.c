#include "adc.h"
#include "button.h"
#include "driverlib/timer.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "lcd.h"
#include "mux.h"

// The voltage value
float voltage = 0.00;

// The current range
int range = 2;

// The mux state
extern int mux_state;

// The current display value
int display_val = 0;


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

    while(SSIBusy(SSI1_BASE)) {
    }

    SSIDataGet(SSI1_BASE, &data_buff[0]);

    while(SSIBusy(SSI1_BASE)) {
    }

    display_val =  data_buff[0];
}

/**
 * Initialize the ADC
 */
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


/** 
 * Unlock the pin 0 on port F 
 */
void unlock_portF_pins() {
    HWREG(GPIO_PORTF_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE+GPIO_O_CR)  |= GPIO_PIN_0;
}

/**
 * Initializes the adc using SPI1
 */
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
