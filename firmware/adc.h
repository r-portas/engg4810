#ifndef ADC_H
#define ADC_H

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

// ADC pins
#define ADC_SSI_BASE            SSI0_BASE
#define ADC_SSI_SYSCTL_PERIPH   SYSCTL_PERIPH_SSI0
#define ADC_SCLK_PIN  GPIO_PIN_2           //output
#define CS_PIN   GPIO_PIN_3           //command/ control out
#define RX_PIN   GPIO_PIN_4           // Set pin as input for
#define TX_PIN   GPIO_PIN_5
#define ADC_PORT SYSCTL_PERIPH_GPIOA
#define ADC_PINS ADC_SCLK_PIN | CS_PIN | DATA_IN_PIN

// GPIO for SSI pins
#define ADC_GPIO_PORT_BASE      GPIO_PORTA_BASE
#define ADC_GPIO_SYSCTL_PERIPH  SYSCTL_PERIPH_GPIOA

extern void init_adc();
extern void adc_read();
extern void roy_adc();
extern int display_val;
extern uint32_t data_buff[2000];
extern int sample_count;

#endif
