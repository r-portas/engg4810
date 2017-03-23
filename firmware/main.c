#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h" 
#include "driverlib/sysctl.h"
#include "driverlib/rom_map.h"
#include "lcd.h"

int main()
{
    initLCD();
    storeSpecialChar();
    while(1) {
        //clearLCD();
        sendSpecialChar();
        // sending the position of
        sendByte(0x00, TRUE);
        //printLCD("Hello World !!");
        Delay(10000000);
    }
}
