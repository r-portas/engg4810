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
int sample_rate[] = {5, 10, 50, 100, 600, 1200, 3000, 6000};
char *sample_msg[] = {"2 read /s", "1 read/ s", "1 read/ 2s", "1 read/ 5s",
                    "1 read/ min", "1 read/ 2 min" , "1 read/5 min",
                    "1 read/ 10min" };

char *ask_prompt = "Select samples : ";
char *ask_samples[] = {"1", "2", "3,", "4", "5", "10", "15", "20", "50", "100", "200", "500" , "1000"};
int test_count = 0;

int sd_samples = 0;
int sd_state = 0;
int sd_sample_index = 0;
int samples_written = 0;
volatile int buzzer_ticks = 0;
int buzzer_state = 0;
int ac_read = 0;
int zero_crossing[100];
int zero_count = 0;

//data_buff[sample_count];
void find_zero_crossings() {
    int i = 0;
    for (i = 0; i < sample_count; i++) {
        //UARTprintf("%d %d\n", data_buff[i], i);
        if ((data_buff[i] <= 0) && (data_buff[i + 1] > 0))
            // storing the positions of the sample count
          // UARTprintf("get in here\n");
           zero_crossing[zero_count] = i;
    }
    // find the number corresponding to the count
    int start = zero_crossing[0];
    int end = zero_crossing[1];
    int count = start - end; // n values
    float volts = 0.00;
    for (i = start; i  < end; i++) {
        UARTprintf("getting voltage\n i : %d", i);
        float this_volt = get_voltage(data_buff[i]);
        volts = volts + ((this_volt) * (this_volt));
    }
    UARTprintf("voltage %d %d", volts, count);
}

void SysTickInt(void)
{
  uint32_t status = 0;
  status = TimerIntStatus(TIMER5_BASE, true);
  TimerIntClear(TIMER5_BASE, status);
  count_ticks++;
  buzzer_ticks++;
  disk_timerproc(); // timer to keep the sd card going
  // DO NOT REMOVE PRINT STATEMENT FOR DEBUGGING
  // UARTprintf("check %d -> %d\n", count_ticks, sample_rate[sample_index]);
      if (count_ticks > sample_rate[sample_index]) {
          count_ticks = 0;
          if (!ac_read) {
              adc_read();
              if (sd_state == 1) {
                   UARTprintf("Writing file interrupt\n");
                   write_file();
                   UARTprintf("Finished writting file in interrupt\n");
                   samples_written++;
                   // have written  the required samples
                   /*if (samples_written == sample_rate[sample_index = '0']) {
                      sd_state = 0;
                   }*/
              }
              update_hardware();
              test_count++;
          } else if (ac_read) {
              find_zero_crossings();
          }
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
        printLCD(ask_samples[sd_samples]);
        lcd_tick = 0;
    }
}

int ac_tick = 0;
void buttonInterrupt() {
   button_tick++;
   lcd_tick++;
   buzzer_ticks++;
   ac_tick++;
   // should be going super fast
   /* if (button_tick == 6000) { */
    check_buttons();
        /* button_tick = 0; */
    /* } */


   // Should not be in a interrupt
    /* if (lcd_tick > 25000) { */
      // UARTprintf("Writing to LCD, state %d\n", my_state);
   clearLCD();
   update_lcd();
    /* } */

    // update this number
    if ((buzzer_ticks > 50) && (buzzer_state == 1)) {
        millis ^= 1;
        if (millis == 0) {
                 GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4 , GPIO_PIN_4);
             } else {
                 GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4 , 0);
             }
        buzzer_ticks = 0;
    }

    if (ac_read) {
       // if (ac_tick > 1) {
         //   ac_tick = 0;
         adc_read();
       // }
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

/* void ButtonInterrupt() */
/* { */
/*   SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER4); */
/*   TimerConfigure(TIMER4_BASE, TIMER_CFG_PERIODIC);   // 32 bits Time */
/*   unsigned long ulPeriod; */
/*   unsigned int Hz = 1;   // frequency in Hz */
/*   ulPeriod = (SysCtlClockGet() / Hz)/ 2; */
/*   TimerLoadSet(TIMER4_BASE, TIMER_A, ulPeriod -1); */
/*   TimerIntRegister(TIMER4_BASE, TIMER_A, buttonInterrupt);    // Registering  isr */
/*   TimerIntEnable(TIMER4_BASE, TIMER_TIMA_TIMEOUT); */
/*   TimerEnable(TIMER4_BASE, TIMER_A); */
/* } */

void init_timers() {
    initTimer();
    /* ButtonInterrupt(); */
}
