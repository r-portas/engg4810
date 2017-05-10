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

#define STATE_SELECTION 9
#define STATE_SD    10
#define STATE_MEASURE   11
#define ASK_SAMPLES 8


#define VOLTMETER   12
#define AMPMETER    13
#define OHMETER     14
#define LOGIC       15
#define NONE        16

/** external variables*/
extern char *message[];
extern int msg_count;
extern char *msgUpdate [];
extern int my_mode;
extern int my_state;

extern void init_buttons();
extern void check_buttons();

#endif
