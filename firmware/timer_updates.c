#include "timer_updates.h"
#include "uart.h"
#include "adc.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "utils/fatfs/src/ff.h"
#include "utils/fatfs/src/diskio.h"

volatile uint32_t millis = 0;
volatile int count_ticks = 0;

// change values on button read
void SysTickInt(void)
{
  uint32_t status = 0;
  status = TimerIntStatus(TIMER5_BASE, true);
  TimerIntClear(TIMER5_BASE, status);
  count_ticks++;
  if (count_ticks == 1) {
      count_ticks = 0;
      millis ^= 1;
   // toggle the pin
      if (millis == 0) {
          GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4 , GPIO_PIN_4);
      } else {
          GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4 , 0);
      }
      //adc_read();
  }
}

void my_delay() {
    // wait for a particular time then read ADC
    // update time on button push
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

// the FATFS needs a 10ms tick
void sd_ticks() {
    // ensure a 10 msec tick
    disk_timerproc();
}

void sd_timer() {
     SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER4);
     TimerConfigure(TIMER4_BASE, TIMER_CFG_PERIODIC);   // 32 bits Time
     unsigned long ulPeriod;
     unsigned int Hz = 100;   // frequency in Hz (10msec tick)
     ulPeriod = (SysCtlClockGet() / Hz)/ 2;
     TimerLoadSet(TIMER4_BASE, TIMER_A, ulPeriod -1);
     TimerIntRegister(TIMER4_BASE, TIMER_A, sd_ticks);    // Registering  isr
     TimerIntEnable(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
     TimerEnable(TIMER4_BASE, TIMER_A);
}
