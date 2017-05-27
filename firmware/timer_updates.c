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

int sample_rate[] = {1500, 3000, 6000, 15000, 30000, 180000, 360000, 900000, 1800000};
char *sample_msg[] = {"2 read / s", "1 read/ s", "1 read/2s", "1 read/ 5s", "1 read/10 s"
                    "1 read/ min", "1 read/ 2 min" , "1 read/5 min",
                    "1 read/10 min" };


char *ask_prompt = "Select samples : ";
char *ask_samples[] = {"1", "2", "3,", "4", "5", "10", "15", "20", "50", "100", "200", "500" , "1000"};
int sample_list[] = { 1, 2, 3, 4, 5, 10, 15, 20, 50, 100, 200, 500, 1000};
int test_count = 0;
int sd_samples = 0;
int sd_sample_index = 0;
int sd_samples_ask = 0;
int sd_state = 0;
int samples_written = 0;

int my_flag = 0;

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
int lcd_ticks = 0;
int button_flag = 0;
int time_count = 0;
int time_sample = 0;
int pc_tick = 0;
int pc_flag = 0;
int light_tick = 0;
int lcd_on_flag = 0;
int lcd_off_flag = 0;
int char_my_mode = 0;
int back_light_num = 0;

/** update the modes print on LCD **/
static void print_mode() {
    if (my_mode == VOLTMETER) {
        printLCD(" V ");
        char_my_mode = 'V';
    } else if (my_mode == AMPMETER) {
        printLCD (" A ");
        char_my_mode = 'A';
    } else if (my_mode == OHMETER) {
        printLCD (" O ");
        char_my_mode = 'O';
    } else if (my_mode == LOGIC) {
        printLCD (" L ");
        char_my_mode = 'L';
    }
}

void SysTickInt(void)
{
    IntMasterDisable();
    uint32_t status = 0;
    status = TimerIntStatus(TIMER5_BASE, true);
    TimerIntClear(TIMER5_BASE, status);

    count_ticks++;
    buzzer_ticks++;
    lcd_ticks++;
    button_tick++;
    pc_tick++;
    light_tick++;
    disk_timerproc(); // timer to keep the sd card going

    if (ac_set == 1) {
      adc_read();
      rms_flag = 1;
      toggle_pin();

    } else if (ac_set == 0) {
        if (count_ticks > sample_rate[sample_index]) {
             count_ticks = 0;
             toggle_pin();
             adc_read();
        }
    }

  /** update the lcd every so often 0.5 sec**/
      if (lcd_ticks > 1000) {
          lcd_flag = 1;
          lcd_ticks = 0;
          my_flag = 1;
      }
  /** checks the button every so often **/
  if (button_tick > 1200) {
      button_tick = 0;
      button_flag = 1;
  }

  if (pc_tick > 1000) {
      pc_tick = 0;
      pc_flag = 1;
  }

  /** for the light pwm  **/
  if (light_tick < back_light_num) {
      //lcd_on_flag = 1;
      back_light_on();
  } else if (light_tick > back_light_num) {
      //lcd_off_flag = 1;
      back_light_off();
      if (light_tick > 100) {
            light_tick = 0;
      }
  }
  IntMasterEnable();
}

int num1= 0;
int left1 = 0;
uint32_t lcd_tick = 0;
extern int display_val;
char buffer[20];

/** Updates the lcd**/
void update_lcd() {
    if (my_state == STATE_MEASURE) {
        if (ac_set) {
         final_2 = sqrt((running_volt/n));
         final_2 = final_2 * 1000;
         num1 = final_2 / 1000;
         left1 = final_2 - (num1 * 1000);
         sprintf(buffer, " %d.%d",num1, left1);
         running_volt = 0.0;
         n = 0;
       } else {
           float final = get_voltage(display_val);
           final = final * 1000;
           num1 = final / 1000;
           left1 = final - (num1 * 1000);
           sprintf(buffer, " %d.%d", num1, left1);
       }
       sendSpecialChar();
       sendByte(0x00, lcd_true);
       if (ac_set) {
           printLCD("AC");

       } else {
           printLCD("DC");
       }
       print_mode();
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
    if (my_state == ASK_SAMPLES) {
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
   //button_tick++;
   buzzer_ticks++;

   /** check for button press **/
   if (button_flag) {
       check_buttons();
       button_flag = 0;
   }
   // Should not be in a interrupt
    if (lcd_flag) {
       clearLCD();
       update_lcd();
       lcd_flag = 0;
    }

    if (my_flag) {
        write_file();
        my_flag = 0;
    }

    if (rms_flag) {
        update_buffer_rms();
        rms_flag = 0;
    }

    if (pc_flag) {
        send_pc();
        pc_flag = 0;
    }
    /*
    if (lcd_on_flag) {
        back_light_on();
        lcd_on_flag = 0;
    }

    if (lcd_off_flag) {
        back_light_off();
        lcd_off_flag = 0;
    }*/
}

void initTimer()
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER5))
  {

  }
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
