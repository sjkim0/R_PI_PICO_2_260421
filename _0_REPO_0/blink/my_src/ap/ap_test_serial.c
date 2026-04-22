#include "ap_test_serial.h"
#include "ap_task.h"
#include <stdio.h>
#include <string.h>


enum AP_TEST_SERIAL_TASK
{
    AP_TEST_SERIAL_TASK_TX_TEST = 0,
    AP_TEST_SERIAL_TASK_LENGTH
};

typedef struct 
{
    /* data */
    uint8_t serial_write_buff[DEF_TEST_SERIAL_WRITE_BUFF_LENGTH];
    uint8_t serial_write_buff_len;
    uint8_t serial_read_buff[DEF_TEST_SERIAL_READ_BUFF_LENGTH];
    uint8_t serial_read_buff_len;

    uint32_t count;
}ap_test_serial_t;


static ap_test_serial_t ap_test_serial_inst;
static ap_task_t ap_test_serial_task[AP_TEST_SERIAL_TASK_LENGTH];

static void _apTestSerialTxTask(void);

void apTestSerialInit(void)
{
    stdio_init_all();
    apTaskInit(true, &ap_test_serial_task[AP_TEST_SERIAL_TASK_TX_TEST], 1000, _apTestSerialTxTask);
}

void apTestSerialLoop(void)
{
    for (uint32_t i = 0; i < AP_TEST_SERIAL_TASK_LENGTH; i++)
    {
        apTaskRun(&ap_test_serial_task[i]);
    }
}

static void _apTestSerialTxTask(void)
{    
    sprintf((char*)ap_test_serial_inst.serial_write_buff, 
            "Hello, world! %d\n", 
            ap_test_serial_inst.count++);
    printf((char*)ap_test_serial_inst.serial_write_buff);
}