#include "sd_card.h"
#include "utils/fatfs/src/diskio.h"
int sd_close = 0;
// internal variables

FIL fil;
FRESULT res;

void make_file() {
    res = f_open(&fil, "test1.txt", FA_CREATE_ALWAYS|FA_WRITE);
    if(res != FR_OK)
    {
        UARTprintf("Error creating file data.txt: %d\n", res);
    }
    else
    {
        UARTprintf("\nCreated file data.txt\n");
    }
}

/** write to the sd card**/
void write_file() {
    IntMasterDisable();
    int bytes_written = 0;

    // Write something
    f_write(&fil, "ABC\n", 4, &bytes_written);
    //fprintf(&fil, "hello\n");
    UARTprintf("bytes %d", bytes_written);
    f_sync(&fil);

    IntMasterEnable();
}

/** close the sd card **/
void close_file() {
    IntMasterDisable();
    res = f_close(&fil);
    f_mount(0, NULL);
    // flush the data
    UARTprintf("res %d", res);
    IntMasterEnable();
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
    write_file();
    //close_file();
}
