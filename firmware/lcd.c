#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom_map.h"
#include "lcd.h"
#include "uart.h"
#include "utils/uartstdio.h"

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
    GPIOPinWrite(CMD_PORT, RS, RS);
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
    sendByte(0x0C, lcd_false);
}


void position_cursor(int x, int y) {
    int address;
   if (x == 0) {
       address = 0;
   } else if (x == 1) {
       address = 0x40;
   } else {
       address = 0;
   }

   address |= y;
   sendByte(0x80 | address, lcd_false);
}

// clear the LCD
void clearLCD(void)
{
    sendByte(0x01, lcd_false); // Clear screen
    sendByte(0x02, lcd_false); // Back to home
    Delay(30000);
}

// Initialize the LCD
void init_LCD(void)
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
    sendByte(0x28,lcd_false);  // Set two lines
    cursorOffLCD();       // Cursor invisible
    sendByte(0x06, lcd_false); // Set insert mode
    clearLCD();
    storeSpecialChar();
    Delay(10000);
}


// Print the text on the screen
void printLCD(char *text)
{
    char *c;
    c = text;

    while ((c != 0) && (*c != 0))
    {
        sendByte(*c, lcd_true);
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

char ohmChar[8] = {
        0b00000,
        0b00000,
        0b01110,
        0b10001,
        0b10001,
        0b01010,
        0b11011,
        0b00000
};

char bright_level_1[8] = {
        0b00001,
        0b00011,
        0b00111,
        0b01111,
        0b01111,
        0b11111,
        0b11111,
        0b11111
};

char bright_level_2[8] = {
        0b00001,
        0b00001,
        0b00011,
        0b00011,
        0b00111,
        0b00111,
        0b01111,
        0b11111
};

char bright_level_3[8] = {
        0b00001,
        0b00001,
        0b00001,
        0b00011,
        0b00011,
        0b00111,
        0b00111,
        0b01111
};

char bright_level_4[8] = {
        0b00000,
        0b00000,
        0b00001,
        0b00001,
        0b00011,
        0b00111,
        0b01111
};

char bright_level_5[8] = {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00011,
        0b01111
};


void storeSpecialChar() {
    // select the CGRAM
    sendByte(0x40, lcd_false);
    // send the byte
    int i;
    for (i = 0 ;i < 8; i++) {
        sendByte(customChar[i], lcd_true);
    }
    // switch back to DDRAM for LCD display
}

void sendSpecialChar() {
    sendByte(0x80, lcd_false);
}

void back_light_off() {
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2 , 0);
}

void back_light_on() {
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2 , GPIO_PIN_2);
}

/** Init the back Light **/
void init_backlight() {
    // Init the back light for tiva (has to be toggled)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_2);
}
