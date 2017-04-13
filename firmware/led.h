
#ifndef LED_H
#define LED_H
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h" 
#include "driverlib/sysctl.h"
#include "driverlib/rom_map.h"

#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

void initLed();

void ledOff();

void ledOn(int color);
#endif
