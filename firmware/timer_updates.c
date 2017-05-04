#include "timer_updates.h"
#include "uart.h"
#include "adc.h"
#include "button.h"
#include "lcd.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "utils/fatfs/src/ff.h"
#include "utils/fatfs/src/diskio.h"
volatile uint32_t millis = 0;
volatile int count_ticks = 0;
long button_tick = 0;

// change values on button read
int sample_index = 0;
int sample_rate[] = {5,10,50,100, 600, 1200, 3000, 6000};
int test_count = 0;


void SysTickInt(void)
{
  uint32_t status = 0;
  status = TimerIntStatus(TIMER5_BASE, true);
  TimerIntClear(TIMER5_BASE, status);
  count_ticks++;
  disk_timerproc();
  //if (count_ticks == 100) {
  if (count_ticks == sample_rate[sample_index]) {
      // for debugging
      count_ticks = 0;
      millis ^= 1;
      if (millis == 0) {
          GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4 , GPIO_PIN_4);
      } else {
          GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4 , 0);
      }
      adc_read();
      test_count++;
      UARTprintf("DO NOT DELETE: test count %d\n", test_count);
     }
}

uint32_t lcd_tick = 0;
void buttonInterrupt() {
   button_tick++;
   lcd_tick++;
   if (button_tick == 12000) {
        check_buttons();
        button_tick = 0;
    }
    if (lcd_tick > 25000) {
       UARTprintf("Writing to LCD, state %d\n", my_state);
       clearLCD();
       /*if (my_state == STATE_MEASURE) {
           char buffer[20];
           sprintf(buffer, "test %d   ", test_count);
           printLCD(buffer);
           lcd_tick = 0;
       } else
       */
       if (my_state == NONE) {
           printLCD(message[msg_count]);
           lcd_tick = 0;
       }
       if (my_state == STATE_SELECTION) {
           printLCD(msgUpdate[msg_count]);
           lcd_tick = 0;
       }

    }
}

void initTimer()
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);
  TimerConfigure(TIMER5_BASE, TIMER_CFG_PERIODIC);   // 32 bits Time
  unsigned long ulPeriod;
  unsigned int Hz = 10;   // frequency in Hz
  ulPeriod = (SysCtlClockGet() / Hz)/ 2;
  TimerLoadSet(TIMER5_BASE, TIMER_A, ulPeriod -1);
  TimerIntRegister(TIMER5_BASE, TIMER_A, SysTickInt);    // Registering  isr
  TimerIntEnable(TIMER5_BASE, TIMER_TIMA_TIMEOUT);
  TimerEnable(TIMER5_BASE, TIMER_A);
}

void ButtonInterrupt()
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER4);
  TimerConfigure(TIMER4_BASE, TIMER_CFG_PERIODIC);   // 32 bits Time
  unsigned long ulPeriod;
  unsigned int Hz = 1;   // frequency in Hz
  ulPeriod = (SysCtlClockGet() / Hz)/ 2;
  TimerLoadSet(TIMER4_BASE, TIMER_A, ulPeriod -1);
  TimerIntRegister(TIMER4_BASE, TIMER_A, buttonInterrupt);    // Registering  isr
  TimerIntEnable(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
  TimerEnable(TIMER4_BASE, TIMER_A);
}


void init_timers() {
    initTimer();
    ButtonInterrupt();
}