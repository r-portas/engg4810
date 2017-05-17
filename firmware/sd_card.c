#include "sd_card.h"
#include "utils/fatfs/src/diskio.h"
#include "timer_updates.h"
int sd_close = 0;
// internal variables
FIL fil;
FATFS fs;
FRESULT res;

// Stores the current sample number
int sampleNum = 1;

void make_file() {
    // TODO: make the filename a parameter to the function
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

void write_line(char* line) {
    IntMasterDisable();
    int bytes_written = 0;

    // Write something
    f_write(&fil, line, strlen(line), &bytes_written);
    UARTprintf("bytes %d\n", bytes_written);
    f_sync(&fil);

    IntMasterEnable();
}

/**
 * Writes a line in the log file
 *
 */
void write_log_line(float reading, char mode) {
    IntMasterDisable();
    int bytes_written = 0;

    char output[64];

    snprintf(output, 64, "%i,%f,%c\n", sampleNum, reading, mode);

    sampleNum++;

    // Write something
    UARTprintf(output);
    f_write(&fil, output, strlen(output), &bytes_written);
    //fprintf(&fil, "hello\n");
    UARTprintf("bytes %d\n", bytes_written);
    f_sync(&fil);

    IntMasterEnable();
}

/** write to the sd card**/
void write_file() {
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4 , GPIO_PIN_4);
    IntMasterDisable();
    int bytes_written = 0;

    // Write something
    f_write(&fil, "ABC\n", 4, &bytes_written);
    //fprintf(&fil, "hello\n");
    UARTprintf("bytes %d\n", bytes_written);
    f_sync(&fil);
    IntMasterEnable();
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4 , 0);
}

void init_sd_card() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
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
}
