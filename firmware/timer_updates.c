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
#include "math.h"
volatile uint32_t millis = 0;
volatile int count_ticks = 0;
long button_tick = 0;

// change values on button read
int sample_index = 0;
int sample_rate[] = {5, 10, 50, 100, 600, 1200, 3000, 6000};
char *sample_msg[] = {"2 read /s", "1 read/ s", "1 read/ 2s", "1 read/ 5s",
                    "1 read/ min", "1 read/ 2 min" , "1 read/5 min",
                    "1 read/ 10min" };


char *ask_prompt = "Select samples : ";
char *ask_samples[] = {"1", "2", "3,", "4", "5", "10", "15", "20", "50", "100", "200", "500" , "1000"};
int sample_list[] = {1,2,3,4,5,10,15,20,50,100,200,500,1000};
int test_count = 0;

int sd_sample_index = 0;
int sd_samples_ask = 0;
int sd_state = 0;
int samples_written = 0;



volatile int buzzer_ticks = 0;
int buzzer_state = 0;
int ac_read = 0;
int zero_crossing[100];
int zero_count = 0;
extern sd_flag;
int lcd_flag = 0;
int ac_set = 1;
float running_volt = 0.00;
int n  = 0;
float final_2;
void SysTickInt(void)
{
  uint32_t status = 0;
  status = TimerIntStatus(TIMER5_BASE, true);
  TimerIntClear(TIMER5_BASE, status);
  count_ticks++;
  buzzer_ticks++;
  disk_timerproc(); // timer to keep the sd card going
  if (ac_set) {
      adc_read();
      // get voltage for the running average
      float raw_volt = get_voltage(display_val);
      running_volt = running_volt + (raw_volt * raw_volt);
      n++;
      //float final = ((sqrt(running_volt)));
      final_2 = sqrt((running_volt/n));
      //UARTprintf("running float %d %d %d %d", (int)raw_volt, (int)running_volt, n, (int)final_2);
  }
  if (count_ticks > sample_rate[sample_index]) {
  /*    count_ticks = 0;
      adc_read();
      update_hardware();
      lcd_flag = 1;
      */
       final_2 = final_2 * 1000;
       int num1 = final_2 / 1000;
       int left1 = final_2 - (num * 1000);
       UARTprintf("vol %d.%d \r",num1, left1);
       running_volt = 0.0;
      n = 0;

  }
}

uint32_t lcd_tick = 0;
extern int display_val;
/** Updates the lcd**/
void update_lcd() {
    // measurement state
    if (my_state == STATE_MEASURE) {
       char buffer[20];
       //sprintf(buffer, "test %d.%d", num, left);
       sprintf(buffer, "test %d", display_val);
       printLCD(buffer);
       position_cursor(1,0);
       printLCD(sample_msg[sample_index]);
       if (sd_state == 1) {
           printLCD(" SD : %");
       }
       lcd_tick = 0;
    }
    // default state
    if (my_state == NONE) {
       printLCD(message[msg_count]);
       lcd_tick = 0;
    }
    // ask to make a selection
    if (my_state == STATE_SELECTION) {
       printLCD(msgUpdate[msg_count]);
       lcd_tick = 0;
    }
    // ask samples
    if (my_state == ASK_SAMPLES){
        printLCD(ask_prompt);
        position_cursor(1,0);
        printLCD(ask_samples[sd_sample_index]);
        lcd_tick = 0;
    }
}

int ac_tick = 0;
void play_buzzer() {
    // plays the buzzer
    if ((buzzer_ticks > 50) && (buzzer_state == 1)) {
           millis ^= 1;
           if (millis == 0) {
                    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4 , GPIO_PIN_4);
                } else {
                    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4 , 0);
                }
           buzzer_ticks = 0;
    }
}

void buttonInterrupt() {
   button_tick++;
   buzzer_ticks++;
   // should be going super fast
   if (button_tick > 7000) {
       check_buttons();
       button_tick = 0;
   }

   // Should not be in a interrupt
    if (lcd_flag) {
       clearLCD();
       update_lcd();
       lcd_flag = 0;
    }
    // update this number
    /*if(sd_state) {
        write_file();
    }*/

}

void initTimer()
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);
  TimerConfigure(TIMER5_BASE, TIMER_CFG_PERIODIC);   // 32 bits Time
  unsigned long ulPeriod;
  unsigned int Hz = 1000;   // frequency in Hz
  ulPeriod = (SysCtlClockGet() / Hz)/ 2;
  TimerLoadSet(TIMER5_BASE, TIMER_A, ulPeriod -1);
  TimerIntRegister(TIMER5_BASE, TIMER_A, SysTickInt);    // Registering  isr
  TimerIntEnable(TIMER5_BASE, TIMER_TIMA_TIMEOUT);
  TimerEnable(TIMER5_BASE, TIMER_A);
}

void init_timers() {
    initTimer();
}
