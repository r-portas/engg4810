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
int rms_flag = 0;


void toggle_pin();

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
      rms_flag = 1;
      // get voltage for the running average
      // UARTprintf("running float %d %d %d %d", (int)raw_volt, (int)running_volt, n, (int)final_2);
  }
  if (count_ticks > 1000) {
      lcd_flag = 1;
      count_ticks = 0;
  }
}

int num1= 0;
int left1 = 0;
uint32_t lcd_tick = 0;
extern int display_val;
/** Updates the lcd**/
void update_lcd() {
    char buffer[20];
    // measurement state
    if (my_state == STATE_MEASURE) {
       if (ac_set) {
         final_2 = sqrt((running_volt/n));
         final_2 = final_2 * 1000;
         num1 = final_2 / 1000;
         left1 = final_2 - (num1 * 1000);
         //UARTprintf("vol %d . %d \r",num1, left1);
         sprintf(buffer, "vol %d.%d",num1, left1);
         UARTprintf("\n r %d %d \n", data_buff[sample_count], sample_count);
         running_volt = 0.0;
         n = 0;
       }
       //sprintf(buffer, "test %d.%d", num, left);
       //sprintf(buffer, "test %d", display_val);
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


void toggle_pin() {
    millis ^= 1;
  if (millis == 0) {
           GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4 , GPIO_PIN_4);
       } else {
           GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4 , 0);
       }
}

void update_buffer_rms() {
    // update the rms value here
   float raw_volt = get_voltage(display_val);
   running_volt = running_volt + (raw_volt * raw_volt);
   n++;
}

void buttonInterrupt() {
   button_tick++;
   buzzer_ticks++;
   //toggle_pin();
   // should be going super fast
   if (button_tick > 1000) {
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

    if (rms_flag) {
        update_buffer_rms();
        rms_flag = 0;
    }
}

void initTimer()
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);
  TimerConfigure(TIMER5_BASE, TIMER_CFG_PERIODIC);   // 32 bits Time
  unsigned long ulPeriod;
  unsigned int Hz = 3000;   // frequency in Hz
  ulPeriod = (SysCtlClockGet() / Hz)/ 2;
  TimerLoadSet(TIMER5_BASE, TIMER_A, ulPeriod -1);
  TimerIntRegister(TIMER5_BASE, TIMER_A, SysTickInt);    // Registering  isr
  TimerIntEnable(TIMER5_BASE, TIMER_TIMA_TIMEOUT);
  TimerEnable(TIMER5_BASE, TIMER_A);
}

void init_timers() {
    initTimer();
}
