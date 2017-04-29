#include "sd_card.h"
#include "utils/fatfs/src/diskio.h"
#include "utils/fatfs/src/ff.h"

// attempting to use the FatFs Libray instead for the sd card

void init_sd_card() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);

    // The sd card requires internal 100 Hz timer for functionality
    SysTickPeriodSet(SysCtlClockGet() / 100);
    SysTickEnable();
    SysTickIntEnable();
    IntMasterEnable();


    static FATFS fs;
    static FIL fil;
    DSTATUS errd;

    FRESULT res = f_mount(0, &fs);
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
    if(a) {
            send_initial_clock_train();
            errd = disk_initialize(0);
            UARTprintf("\nInitialising disk 0. Status = %i\n", errd);
        }
       res = f_open(&fil, "data.txt", FA_CREATE_ALWAYS|FA_WRITE);
       if(res != FR_OK) {
           UARTprintf("Error creating file data.txt: %d\n", res);
       } else {
           UARTprintf("\nCreated file data.txt\n");
       }
       int bytes_written = 0;
       // Write something
       f_write(&fil, "ABC123!", 7 , &bytes_written);
       UARTprintf("Written %d bytes\n", bytes_written);
       f_close(&fil);
}
