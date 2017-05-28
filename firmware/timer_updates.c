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
                    "1 r/min", "1 r/2 min" , "1 r/5 min",
                    "1 r /10 min" };


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
extern int sd_flag;
int lcd_flag = 0;
int ac_set = 0;
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
int back_light_num = 100;
int sd_ticks = 0;
float global_voltage = 0.00;
int buzzer_flag = 0;

/*** Plays the buzzer ***/
void play_buzzer() {
    buzzer_state = 1;
    // plays the buzzer
    if (buzzer_state == 1) {
           millis ^= 1;
       if (millis == 0) {
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4 , GPIO_PIN_4);
            } else {
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4 , 0);
       }
    }
}

/** update the modes print on LCD **/
static void print_mode() {
    if (my_mode == VOLTMETER) {
        printLCD("V ");
        char_my_mode = 'V';
    } else if (my_mode == AMPMETER) {
        printLCD ("A ");
        char_my_mode = 'A';
    } else if (my_mode == OHMETER) {
        printLCD ("O ");
        char_my_mode = 'O';
    } else if (my_mode == LOGIC) {
        printLCD ("L ");
        char_my_mode = 'L';
    }
}

/** Interbal timer to control the ticks state **/
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
    sd_ticks++;
    disk_timerproc(); // timer to keep the sd card going

    if (ac_set == 1) {
        if (count_ticks > sample_rate[sample_index]) {
             count_ticks = 0;
             int i = 0;
             int n = 20;

             float total = 0.0;

             for (i = 0; i < n; i++) {

                adc_read();
                total += get_voltage(display_val);

                SysCtlDelay(100);
             }

             global_voltage = sqrtf( total/(float)n );
            char tempBuff[30];
            sprintf(tempBuff, ">>> GV: %.2f, %d\n", total, n);
            UARTprintf("%s", tempBuff);
        }
    }  else if (ac_set == 0) {
        if (count_ticks > sample_rate[sample_index]) {
             count_ticks = 0;
             adc_read();
             global_voltage = get_voltage(display_val);
        }
    }
  /** update the LCD every so often 0.5 sec**/
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

  if (buzzer_ticks > 4) {
      buzzer_ticks = 0;
      play_buzzer();
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


uint32_t lcd_tick = 0;
extern int display_val;
char buffer[100];
char lightBuffer[20];
char temp[20];
char signal_setting[20];


/** Updates the lcd**/
void update_lcd() {
    if (my_mode == BRIGHTNESS) {
        sprintf(lightBuffer, "Level %d", back_light_num/20);
        printLCD(lightBuffer);
        return;
    }

    if (my_mode == VOLTMETER) {
        if (global_voltage > 12.00) {
            printLCD("OVER LIMIT");
            return;
        }
    }

    if (my_state == STATE_MEASURE) {
        if (my_mode == OHMETER) {
            printLCD("OHMS");
            return;
        }
        sprintf(buffer, "%.2f", global_voltage);
        if (my_mode == VOLTMETER) {
            switch(voltage_range) {
                case 1:
                   sprintf(temp, "1V ");
                      break;
                  case 2:
                      sprintf(temp, "5V ");
                      break;
                  case 3:
                      sprintf(temp, "15V ");
                      break;
              }

           } else if (my_mode == AMPMETER) {
               switch(current_range) {
                   case 1:
                       sprintf(temp, "10mA ");
                       break;
                   case 2:
                       sprintf(temp, "200mA ");
                       break;
               }
           }
           if ((my_mode == VOLTMETER) || (my_mode == AMPMETER)) {
               if (ac_set) {
                   sprintf(signal_setting, "AC ");
               } else {
                   sprintf(signal_setting, "DC ");
               }
           }

           print_mode();
           printLCD(temp);
           printLCD(signal_setting);
           sendByte(0x00, lcd_true);
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
    //sendSpecialChar();
    sendByte(0x01, lcd_true);
}


/** update the ac buffer **/
void update_buffer_rms() {
    // update the rms value here
    float raw_voltage = display_val / 65535.00;
    raw_voltage = raw_voltage * 3.3;
    running_volt = running_volt + (raw_voltage * raw_voltage);
    n++;
}

/** Makes the updates for the button Interrupt ***/
void buttonInterrupt() {
   if (buzzer_flag) {
       play_buzzer();
       buzzer_flag = 0;
   }
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
}

/** Init the internal timers **/
void initTimer() {
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

/** Init the timers **/
void init_timers() {
    initTimer();
}
