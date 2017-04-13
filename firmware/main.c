#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_ssi.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/uart.h"
#include "driverlib/rom_map.h"
#include "driverlib/ssi.h"
#include "inc/hw_ssi.h"
#include "utils/uartstdio.h"
#include "lcd.h"
//#include "brd_adc.h"

float voltage = 0.00;
uint32_t result;
char str[10];
char myStr[20];
int decVal;
char decValStr[20];

#define A GPIO_PIN_4 // Pin 5
#define B GPIO_PIN_5 // Pin 6
#define C GPIO_PIN_6 // Pin 23

#define MUX_PINS A | B | C
#define MUX_PORT SYSCTL_PERIPH_GPIOF
#define BUTTON_PORT SYSCTL_PERIPH_GPIO

// ADC pins
#define ADC_SCLK_PIN  GPIO_PIN_2           //output
#define CS_PIN   GPIO_PIN_3           //command/ control out
#define RX_PIN   GPIO_PIN_4           // Set pin as input for
#define TX_PIN   GPIO_PIN_5


#define ADC_PORT SYSCTL_PERIPH_GPIOA
#define ADC_PINS ADC_SCLK_PIN | CS_PIN | DATA_IN_PIN


// Yet, another good itoa implementation
// returns: the length of the number string

int toString(int value, char *sp, int radix)
{
    char tmp[16];// be careful with the length of the buffer
    char *tp = tmp;
    int i;
    unsigned v;

    int sign = (radix == 10 && value < 0);
    if (sign)
        v = -value;
    else
        v = (unsigned)value;

    while (v || tp == tmp)
    {
        i = v % radix;
        v /= radix; // v/=radix uses less CPU clocks than v=v/radix does
        if (i < 10)
          *tp++ = i+'0';
        else
          *tp++ = i + 'a' - 10;
    }

    int len = tp - tmp;

    if (sign)
    {
        *sp++ = '-';
        len++;
    }

    while (tp > tmp)
        *sp++ = *--tp;

    return len;
}

void output_mux(int val) {
    switch (val) {
         case(1) :
                GPIOPinWrite(MUX_PORT, A , 0);
                GPIOPinWrite(MUX_PORT, B , 0);
                GPIOPinWrite(MUX_PORT, C , 0);
                break;
        case (2):
                GPIOPinWrite(MUX_PORT, A , 1);
                GPIOPinWrite(MUX_PORT, B , 0);
                GPIOPinWrite(MUX_PORT, C , 0);
                break;
        case (3):
                GPIOPinWrite(MUX_PORT, A , 0);
                GPIOPinWrite(MUX_PORT, B , 1);
                GPIOPinWrite(MUX_PORT, C , 0);
                break;
        case (4):
                GPIOPinWrite(MUX_PORT, A , 1);
                GPIOPinWrite(MUX_PORT, B , 1);
                GPIOPinWrite(MUX_PORT, C , 0);
                break;
        case (5):
                GPIOPinWrite(MUX_PORT, A , 0);
                GPIOPinWrite(MUX_PORT, B , 0);
                GPIOPinWrite(MUX_PORT, C , 1);
                break;
    }
}

void init_adc() {
    // configure the clock for ADC
        // enable the SSI and GPIO port
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_SSI0)) {

	}
	//SSIClockSourceSet(SYSCTL_PERIPH_SSI0, SSI_CLOCK_SYSTEM);
    SysCtlPeripheralEnable(ADC_PORT);
	SysCtlDelay(3);


	// set up the pins for SSI
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA4_SSI0RX);
    GPIOPinConfigure(GPIO_PA5_SSI0TX);
    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_2);

    // Configure the CS pin as output to select CS
	SysCtlDelay(3);
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3);


	// for TX
	//SysCtlDelay(3);
	//GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_5);

	SysCtlDelay(3);
    // Configure the clock settings
    // SPI mode, master mode, 1MHz SSI frequency, and 8-bit data
    SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                           SSI_MODE_MASTER, 1000000, 16);

    // Link clock and enable SSI for conversion
    SSIEnable(SSI0_BASE);
}

uint32_t DataRx[1000];
#define NUM_SSI_DATA            3
uint32_t pui32DataTx[NUM_SSI_DATA];
uint32_t pui32DataRx[NUM_SSI_DATA];
uint32_t ui32Index;

void debug_adc() {
    while(SSIDataGetNonBlocking(SSI0_BASE, &pui32DataRx[0]))
       {
       }
       for(ui32Index = 0; ui32Index < NUM_SSI_DATA; ui32Index++)
       {
           SSIDataPut(SSI0_BASE, 'a');
       }

}




void adc_read() {
    GPIOPinWrite(GPIO_PORTA_BASE, CS_PIN , CS_PIN);
    SysCtlDelay(35);
    GPIOPinWrite(GPIO_PORTA_BASE, CS_PIN , 0);
    SysCtlDelay(70);
    SSIDataPut(SSI0_BASE, 'D');
    while(SSIBusy(SSI0_BASE))
    {

    }
    SSIDataGet(SSI0_BASE, &pui32DataRx[0]);

}

int main()
{
    initLCD();
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                         SYSCTL_XTAL_16MHZ);
    init_adc();
    //init_special();
    //InitConsole();
    //set_up();
    //storeSpecialChar();
   // ADCIntClear(ADC1_BASE, 3);
    while(1) {
        //sendSpecialChar();
        //adc_debug();
        adc_read();
        printLCD("hello !");
        //adc_debug();
        // sending the position of
        /*sendByte(0x00, TRUE);

        strcpy(myStr, str);
        strcat(myStr, ".");
        strcat(myStr, decValStr);
        printLCD(myStr);
        Delay(10000000);
        */
        // ADC stuff
        /*ADCProcessorTrigger(ADC1_BASE, 3);
        while(!ADCIntStatus(ADC1_BASE, 3, false))
        {

        }
        ADCIntClear(ADC1_BASE, 3);
        ADCSequenceDataGet(ADC1_BASE, 3, &result);
        // convert to voltage and grab value
        voltage = result* 0.000805;
        int volInt = (int)voltage;
        toString(volInt, str, 10);
        decVal = (voltage * 100) - (volInt * 100);
        toString(decVal , decValStr, 10);
        */

    }
}
