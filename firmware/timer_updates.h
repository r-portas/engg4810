#ifndef TIMER_UPDATES_H
#define TIMER_UPDATES_H

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

void init_timers(void);
extern int sampling_index;
void initTimer(void);
void ButtonInterrupt(void);

#endif
