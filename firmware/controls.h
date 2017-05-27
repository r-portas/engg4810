#ifndef CONTROLS_H
#define CONTROLS_H

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#define SHCP GPIO_PIN_6
#define STCP GPIO_PIN_5
#define DS GPIO_PIN_4

// Controls for the hardware interfaces
#define OHM_PORT    GPIO_PORTB_BASE
#define OHM_PERI    SYSCTL_PERIPH_GPIOB
#define OHM_PIN_0   GPIO_PIN_2

#define CONTI_PORT  GPIO_PORTB_BASE
#define CONT_PERIPH SYSCTL_PERIPH_GPIOB
#define CONT_PIN    GPIO_PIN_3

void  init_hardware_control();
extern int sample_count;
extern uint32_t data_buff[2000];
extern float get_voltage(int final);

// strings for updating the lcd
extern char ohm_string[20];
extern int num;
extern int left;
#endif
