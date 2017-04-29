#ifndef MUX_H
#define MUX_H



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

// using the same peripheral as buttons, so don't have to necessarily change that
#define MUX_PORT_BASE GPIO_PORTE_BASE
#define A GPIO_PIN_1
#define B GPIO_PIN_2
#define C GPIO_PIN_3

extern void init_mux();
extern void output_state();

#endif
