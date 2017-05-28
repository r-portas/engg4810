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

// reads the data from the adc
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
