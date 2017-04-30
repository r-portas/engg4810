#include "sd_card.h"
#include "utils/fatfs/src/diskio.h"

// internal variables
static FIL fil;
FRESULT res;

void make_file() {
    res = f_open(&fil, "data.txt", FA_CREATE_ALWAYS|FA_WRITE);
    if(res != FR_OK)
    {
        UARTprintf("Error creating file data.txt: %d\n", res);
    }
    else
    {
        UARTprintf("\nCreated file data.txt\n");
    }
}

void write_file() {
    int bytes_written = 0;
    // Write something
    f_write(&fil, "HELLO!", 6 , &bytes_written);
    UARTprintf("Written %d bytes\n", bytes_written);
    f_close(&fil);
}

void init_sd_card() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    static FATFS fs;
    res = f_mount(0, &fs);
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
    BOOL a = wait_ready();
    DSTATUS errd;
    if(a) {
        send_initial_clock_train();
        errd = disk_initialize(0);
        UARTprintf("\nInitialising disk 0. Status = %i\n", errd);
    }
    make_file();
    write_file();
    UARTprintf("exiting\n");
}
