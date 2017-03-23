#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom_map.h"
#include "lcd.h"



// Pulse the EN bit LOW - HIGH - LOW
// To signal the LCD to accept the data/cmd
void pulseLCD()
{
    // Go Low -> High -> Low
    GPIOPinWrite(CMD_PORT, EN, 0);
    GPIOPinWrite(CMD_PORT, EN, EN);
    GPIOPinWrite(CMD_PORT, EN, 0);
}

// Set the RS to LOW
// Indicating incoming command
void setCmd() {
    GPIOPinWrite(CMD_PORT, RS,0);
}


// Set the RS to HIGH
// Indicating incoming data
void setData() {
    GPIOPinWrite(CMD_PORT, RS,RS);
}


// Send data byte in 4 bit mode
// High nibble is sent first
void sendByte(char byteToSend, int isData)
{
    if (isData)
        setData();
    else
        setCmd();
    Delay(400);
    GPIOPinWrite(DATA_PORT, ALLDATAPINS, byteToSend >>4);
    pulseLCD();
    GPIOPinWrite(DATA_PORT, ALLDATAPINS, byteToSend);
    pulseLCD();
}

// Hide cursor from screen
void cursorOffLCD(void) {
    sendByte(0x0C, FALSE);
}



// clear the LCD
void clearLCD(void)
{
    sendByte(0x01, FALSE); // Clear screen
    sendByte(0x02, FALSE); // Back to home
    Delay(30000);
}

// Initialize the LCD
void initLCD(void)
{
    //
    // set the MSP pin configurations
    // and bring them to low
    //
    SysCtlPeripheralEnable(DATA_PERIPH);
    SysCtlPeripheralEnable(CMD_PERIPH);
    GPIOPinTypeGPIOOutput(DATA_PORT,  ALLDATAPINS);
    GPIOPinTypeGPIOOutput(CMD_PORT, ALLCONTROLPINS);
    GPIOPinWrite(DATA_PORT, ALLDATAPINS ,0);
    GPIOPinWrite(CMD_PORT, ALLCONTROLPINS ,0);

    // wait for the LCM to warm up and reach
    // active regions. Remember MSPs can power
    // up much faster than the LCM.

    Delay(10000);

    // initialize the LCM module
    // Set 4-bit input
    //
    setCmd();
    Delay(15000);
    GPIOPinWrite(DATA_PORT, ALLDATAPINS, 0b0010);
    pulseLCD();
    GPIOPinWrite(DATA_PORT, ALLDATAPINS, 0b0010);
    pulseLCD();
    sendByte(0x28,FALSE);  // Set two lines
    cursorOffLCD();       // Cursor invisible
    sendByte(0x06, FALSE); // Set insert mode
    clearLCD();
}


// Print the text on the screen
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

char customChar[8] = {
         0b01000,
         0b11101,
         0b01010,
         0b00100,
         0b01000,
         0b10111,
         0b00000,
         0b00000
};

void storeSpecialChar() {
    // select the CGRAM
    sendByte(0x40, FALSE);
    // send the byte
    int i;
    for (i = 0 ;i < 8; i++) {
        sendByte(customChar[i], TRUE);
    }
    // switch back to DDRAM for LCD display
}


void sendSpecialChar() {
    sendByte(0x80, FALSE);
}