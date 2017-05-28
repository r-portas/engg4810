#ifndef UART_H
#define UART_H

#include "adc.h"
#include "button.h"
#include "controls.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "timer_updates.h"
#include "timer_updates.h"
#include "utils/uartstdio.h"
#include <stdbool.h>
#include <stdint.h>

// Change between external and internal uart
#define EXTERNAL_UART 1

void init_uart();

void read_uart();

void send_mode();

void send_pc();

#endif
