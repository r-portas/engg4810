
#ifndef CONTROLS_H
#define CONTROLS_H

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

// Controls for the hardware
#define OHM_PORT    GPIO_PORTB_BASE
#define OHM_PERI    SYSCTL_PERIPH_GPIOB
#define OHM_PIN_0   GPIO_PIN_0

void  init_hardware_control();
extern int sample_count;
extern uint32_t data_buff[1000];


// strings for updating the lcd
extern char ohm_string[20];
extern int num;
extern int left;
#endif
