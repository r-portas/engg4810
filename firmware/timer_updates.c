#include "timer_updates.h"
#include "uart.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

volatile uint32_t millis = 0;
void SysTickInt(void)
{
  uint32_t status = 0;
  status = TimerIntStatus(TIMER5_BASE, true);
  TimerIntClear(TIMER5_BASE, status);
  //UARTprintf("int %d\n", millis);
  millis++;
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
  unsigned int Hz = 1;   // frequency in Hz
  ulPeriod = (SysCtlClockGet() / Hz)/ 2;
  TimerLoadSet(TIMER5_BASE, TIMER_A, ulPeriod -1);
  TimerIntRegister(TIMER5_BASE, TIMER_A, SysTickInt);    // Registering  isr
  TimerIntEnable(TIMER5_BASE, TIMER_TIMA_TIMEOUT);
  TimerEnable(TIMER5_BASE, TIMER_A);
}
