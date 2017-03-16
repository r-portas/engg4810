/*
 * main.c
 */
// should take this over in a .h library
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"


#define RS GPIO_PIN_4 // Energia Pin 5
#define EN GPIO_PIN_5 // Energia Pin 6
#define D4 GPIO_PIN_0 // Energia Pin 23
#define D5 GPIO_PIN_1 // Energia Pin 24
#define D6 GPIO_PIN_2 // Energia Pin 25
#define D7 GPIO_PIN_3 // Energia Pin 26
#define ALLDATAPINS  D7 | D6 | D5 | D4
#define ALLCONTROLPINS RS | EN

#define DATA_PORT_BASE GPIO_PORTD_BASE
#define CMD_PORT_BASE GPIO_PORTE_BASE
#define DATA_PERIPH SYSCTL_PERIPH_GPIOD
#define CMD_PERIPH SYSCTL_PERIPH_GPIOE
#define FALSE 1
#define TRUE 0

// NOTES
// DATA_PORT_BASE --> PORT D
// CMD_PORT_BASE --> PORT E (for RS and EN signals)

// sets the RS pin to low
void setCmd() {
    GPIOPinWrite(CMD_PORT_BASE, RS, 0);
    // gpio write port, pin and then value
}

// sets the RS pin to high
void setData() {
    GPIOPinWrite(CMD_PORT_BASE, RS, 1);
}

void pulseLCD()
{
    // Go Low -> High -> Low
     GPIOPinWrite(CMD_PORT_BASE, EN, 0);
     GPIOPinWrite(CMD_PORT_BASE, EN, EN);
     GPIOPinWrite(CMD_PORT_BASE, EN, 0);
}

void sendByte(char byteToSend, int isData)
{
     if (isData)
         setData();
     else
         setCmd();
     SysCtlDelay(400);
     GPIOPinWrite(DATA_PORT_BASE, ALLDATAPINS, byteToSend >>4);
     pulseLCD();
     GPIOPinWrite(DATA_PORT_BASE, ALLDATAPINS, byteToSend);
     pulseLCD();
}

// switch off the cursor
void cursorOffLCD() {
    // command byte
    sendByte(0x0C,FALSE);
}

// set up the two lines function set and entry mode set
void setUp() {
    sendByte(0x28,FALSE);  // Set two lines - 28 to represent function setup
    cursorOffLCD();       // Cursor invisible
    sendByte(0x06, FALSE); // Set insert mode 06 to represent insert/ entry mode
}

void printLCD(char *text)
{
     char *c;
     c = text;
      while ((c != 0) && (*c != 0))
     {
      sendByte(*c, TRUE);
      c++;
     }
}

void initLCD(void)
{
 SysCtlPeripheralEnable(DATA_PERIPH);   // enables data port (port D)
 SysCtlPeripheralEnable(CMD_PERIPH);    // enables command port (port E)
 GPIOPinTypeGPIOOutput(DATA_PORT_BASE,  ALLDATAPINS);   // set all the data pins high
 GPIOPinTypeGPIOOutput(CMD_PORT_BASE, ALLCONTROLPINS);  // set all the control pins high
 GPIOPinWrite(DATA_PORT_BASE, ALLDATAPINS ,0);  // clear all the data pins
 GPIOPinWrite(CMD_PORT_BASE, ALLCONTROLPINS ,0); // clear all the control pins


 // adds the system delay
 SysCtlDelay(10000);
 setCmd();
 SysCtlDelay(15000);
 GPIOPinWrite(DATA_PORT_BASE, ALLDATAPINS, 0b0010);
 pulseLCD();
 GPIOPinWrite(DATA_PORT_BASE, ALLDATAPINS, 0b0010);
 pulseLCD();

}

int main(void) {
    initLCD();  // set up the ports and pins

    volatile uint32_t ui32Loop;
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }
    pulseLCD(); // sets up the lcd enable pin
    setUp();    // sets up the correct modes
    printLCD("text");
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
    while(1) {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
               for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++)
               {
               }
               GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x0);
               for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++)
               {
               }
    }
}
