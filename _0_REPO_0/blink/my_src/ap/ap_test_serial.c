#include "ap_test_serial.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>


typedef struct 
{
    /* data */
    uint8_t serial_write_buff[DEF_TEST_SERIAL_WRITE_BUFF_LENGTH];
    uint8_t serial_write_buff_len;
    uint8_t serial_read_buff[DEF_TEST_SERIAL_READ_BUFF_LENGTH];
    uint8_t serial_read_buff_len;
}ap_test_serial_t;


void apTestSerialInit(void)
{
    stdio_init_all();
}

void apTestSerialLoop(void)
{
    printf("Hello, world!\n");
    sleep_ms(1000);
}