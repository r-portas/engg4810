#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#define RED_LED GPIO_PIN_1
#define BLUE_LED GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

/**
 * This is called if the TI driverlib encounters an error
 */
#ifdef DEBUG
void __error__(char *pcFilename, uint32_t ui32Line) {
    while(1);
}
#endif

int main(void) {
    volatile uint32_t ui32Loop;

    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //
    // Check if the peripheral access is enabled.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {
    }


    //
    // Enable the GPIO pin for the LED (PF3).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED | GREEN_LED | BLUE_LED);

    uint8_t counter = 0;

    //
    // Loop forever.
    //
    while(1) {
        //
        // Turn on the LED.
        //
        switch (counter) {
            case 0:
                GPIOPinWrite(GPIO_PORTF_BASE, RED_LED | GREEN_LED | BLUE_LED, RED_LED);

                counter++;
                break;

            case 1:
                GPIOPinWrite(GPIO_PORTF_BASE, RED_LED | GREEN_LED | BLUE_LED, GREEN_LED);

                counter++;
                break;

            case 2:
                GPIOPinWrite(GPIO_PORTF_BASE, RED_LED | GREEN_LED | BLUE_LED, BLUE_LED);

                counter = 0;
                break;
        }

        //
        // Delay for a bit.
        //
        for(ui32Loop = 0; ui32Loop < 2000000; ui32Loop++) {
        }

        //
        // Turn off the LED.
        //
        GPIOPinWrite(GPIO_PORTF_BASE, RED_LED | GREEN_LED | BLUE_LED, 0x0);

        //
        // Delay for a bit.
        //
        for(ui32Loop = 0; ui32Loop < 2000000; ui32Loop++) {
        }
    }
}
