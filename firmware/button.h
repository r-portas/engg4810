#ifndef BUTTON_H
#define BUTTON_H

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
#include "utils/uartstdio.h"
#include "utils/fatfs/src/ff.h"
#include "utils/fatfs/src/diskio.h"


extern int mux_state;
#define Button_PERIPH SYSCTL_PERIPH_GPIOE
#define ButtonBase GPIO_PORTE_BASE
#define Button GPIO_PIN_0
#define ButtonInt GPIO_INT_PIN_0

extern void init_interrupt_button();

// NOTES
// leave 4 pins for the mux and name them here


// 4 pins for the different modes for the measurements


// 3 pins for the sd card samling and stuff
#endif
