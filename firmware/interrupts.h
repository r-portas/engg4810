
#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "uart.h"

#define INTERRUPT_PORT GPIO_PORTB_BASE
#define INTERRUPT_PINS GPIO_PIN_1

void initInterrupts();

#endif
