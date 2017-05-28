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
#include "controls.h"
#include "sd_card.h"
#include "lcd.h"

void init_timers(void);
extern int sampling_index;
void initTimer(void);
void buttonInterrupt(void);
extern int sample_index;
extern int ac_state;

// sd card storage
extern int sd_samples;
extern int sd_state;

// flags to control the sd sampling state
extern int sd_sample_index;
extern int sd_samples_ask;
extern int sd_state;
extern int samples_written;
extern char *ask_samples[];
extern int sample_list[];
extern int num1;
extern int left1;
extern int ac_set;
extern int char_my_mode;
extern char *sample_msg[];
extern int sd_sample_index;
extern int buzzer_state;
extern int back_light_num;
extern float global_voltage;

#endif
