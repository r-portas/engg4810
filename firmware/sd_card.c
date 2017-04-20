#include "sd_card.h"
#include "utils/fatfs/src/diskio.h"

#define SD_BASE GPIO_PORTA_BASE
#define SD_CS GPIO_PIN_3

uint32_t exchange_bytes(uint32_t data) {
    UARTprintf("sending %d\n", data);
    uint32_t recv_data = '*';
    SSIDataPut(SSI0_BASE, data);
    while(SSIBusy(SSI0_BASE))
    {

    }
    SSIDataGet(SSI0_BASE, &recv_data);
    UARTprintf("return %d\n", recv_data);
    return recv_data;
}

void init_sd_card() {
    // configure the clock for ADC
           // enable the SSI and GPIO port
       SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
       while(!SysCtlPeripheralReady(SYSCTL_PERIPH_SSI0)) {

       }
       SysCtlPeripheralEnable(SD_BASE);
       SysCtlDelay(3);

       // set up the pins for SSI
       GPIOPinConfigure(GPIO_PA2_SSI0CLK);
       GPIOPinConfigure(GPIO_PA4_SSI0RX);
       GPIOPinConfigure(GPIO_PA5_SSI0TX);
       GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_2);

       // Configure the CS pin as output to select CS
       SysCtlDelay(3);
       GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3);

       SysCtlDelay(3);
       // Configure the clock settings
       // SPI mode, master mode, 1MHz SSI frequency, and 8-bit data
       SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                              SSI_MODE_MASTER, 1000000, 8);

       // Link clock and enable SSI for conversion
       SSIEnable(SSI0_BASE);
}

// Sends a formatted command to the SD card
unsigned char send_command(unsigned char  cmd, unsigned char argument) {
    unsigned int rx;

    // update
    while(SSIDataGetNonBlocking(SSI0_BASE, &rx));
    int updated_cmd = (cmd | 0x40);

    exchange_bytes(updated_cmd);
    exchange_bytes(argument >> 16);
    exchange_bytes(argument);

    if (cmd == CMD0) {
        exchange_bytes(0x95);
    }  else if (cmd == CMD8) {
        exchange_bytes(0x87);
    } else {
        // byte of CRC that we do not care about
        exchange_bytes(0xff);
    }
    int i ;
   // read the response back
   // should receive a valid response back within 0 to 8 bytes for an SD card
    for (i = 0; i < 9; i++) {
        rx = exchange_bytes(0xffff);
        if (rx != 0xffff) {
            UARTprintf("char is %d\n", rx);
            break;
        }
    }
    return rx;
}


// the address has to be 512 bytes block
void write_data(unsigned char *line, unsigned int address) {
    int i;
    unsigned char rx = send_command(CMD24, address);
    if (rx != 0x00) {
        // error
    }
    // send the start token
    exchange_bytes(0xfe);

    // transfer the 512 bytes
    for (i = 0; i < 512; i++) {
        exchange_bytes(line[i]);
    }
    // send the CRC
    exchange_bytes(0xff);
    exchange_bytes(0xff);

    // wait till the end of transmission is detected
    while(exchange_bytes(0xff) != 0xff);
}

// set up the SD card for SPI transactions
void card_setup() {
    // wait for at least 74 clock pulses with DATA and CS set
    // set CS low
    GPIOPinWrite(SD_BASE, SD_CS, 0);

    int i;
    // keep the pin high for 74 clock cycles
    for(i = 0; i < 10; i++) {
            exchange_bytes(0xffff);
    }


    // send command 0x00
    char rx = send_command(CMD0, 0x00);
    // check r1 has no error bits (may have idle 1)

    UARTprintf("from CMD-0 %d\n", rx);
    // wait to wake it up from the idle state
    // send command 1
    // check r1 is not in idle anymore
    while(send_command(CMD1, 0x00) != 0x00);
    UARTprintf("have set CMD-1\n");
}

//FRESULT f_mount (BYTE vol, FATFS* fs);
// attempting to use the FatFs Libray instead for the sd card

void sd_card_attempt2() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    SysTickEnable();
    IntMasterEnable();
    SysTickIntEnable();

    static FATFS fs;
    static FIL fil;
    FRESULT res = f_mount(0, &fs);
    uint32_t count = 8 * 512;

    if(res != FR_OK)
    {
       UARTprintf("f_mount error\n"); //no error message here
    }
    if(res == FR_INVALID_DRIVE)
       {
           UARTprintf("f_mount error\n");
       }
    else if(res == FR_OK)
       {
           UARTprintf("SD card found\n");
       }

    power_on();
    //disk_initialize(0);
   BOOL a = wait_ready();
    DSTATUS errd;
    if(a) {
        send_initial_clock_train();
        errd = disk_initialize(0);
        UARTprintf("\nInitialising disk 0. Status = %i\n", errd);
    }

       res = f_open(&fil, "data.txt", FA_CREATE_ALWAYS|FA_WRITE);
       if(res != FR_OK)
       {
           UARTprintf("Error creating file data.txt: %d\n", res);
       }
       else
       {
           UARTprintf("\nCreated file data.txt\n");
       }

       int bytes_written = 0;

       // Write something
       f_write(&fil, "abc", 3, &bytes_written);

       UARTprintf("Written %d bytes\n", bytes_written);

       f_close(&fil);
}
