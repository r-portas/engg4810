
#ifndef LCD_H
#define LCD_H


#define lcd_false  0
#define lcd_true   1

/*
 *
 * Pin Connections:
 * PE4  -> RS
 * PE5  -> EN
 * PD0  -> D4
 * PD1  -> D5
 * PD2  -> D6
 * PD3  -> D7
 *
*/
#define RS GPIO_PIN_4 // Pin 5
#define EN GPIO_PIN_5 // Pin 6
#define D4 GPIO_PIN_0 // Pin 23
#define D5 GPIO_PIN_1 // Pin 24
#define D6 GPIO_PIN_2 // Pin 25
#define D7 GPIO_PIN_3 // Pin 26
#define ALLDATAPINS  D7 | D6 | D5 | D4
#define ALLCONTROLPINS RS | EN

#define DATA_PORT GPIO_PORTD_BASE
#define CMD_PORT GPIO_PORTE_BASE
#define DATA_PERIPH SYSCTL_PERIPH_GPIOD
#define CMD_PERIPH SYSCTL_PERIPH_GPIOE
#define Delay(num) SysCtlDelay(num)

// pa5, pa6, pa7

extern void init_LCD();
extern void storeSpecialChar();
extern void sendSpecialChar();
extern void sendByte(char byteToSend, int isData);
extern void printLCD(char *text);
extern void clearLCD();

#endif
