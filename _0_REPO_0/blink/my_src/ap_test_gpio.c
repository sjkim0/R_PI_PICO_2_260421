#include "ap_test_gpio.h"
#include "pico.h"
#include "pico/stdio.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include "ap_test_def.h"


typedef struct
{
    /* data */
    bool gpio_read_buff[TEST_GPIO_INPUT_LENGTH - TEST_GPIO_INPUT_0];
}ap_test_t;


static ap_test_t ap_test_inst;

/*
 * @brief: test gpio read and write
 * @note: 6-9 as output, 10-13 as input
 */
static void _testGpio(void);

static void _readGpio(void);
static void _writeGpio(bool value);

void apTestInit(void)
{
    /* gpio init */
    for(int i = TEST_GPIO_OUTPUT_0; i < TEST_GPIO_OUTPUT_LENGTH; i++)
    {
        gpio_init(i);
        gpio_set_dir(i, GPIO_OUT);
    }
    for(int i = TEST_GPIO_INPUT_0; i < TEST_GPIO_INPUT_LENGTH; i++)
    {
        gpio_init(i);
        gpio_set_dir(i, GPIO_IN);
    }
}

void apTestLoop(void)
{
    _testGpio();
}

static void _testGpio(void)
{
    _writeGpio(true);
    _readGpio();
    sleep_ms(DEF_SLEEP_MS_0);
    _writeGpio(false);
    _readGpio();
    sleep_ms(DEF_SLEEP_MS_0);
}

static void _readGpio(void)
{
    for(int i = TEST_GPIO_INPUT_0; i < TEST_GPIO_INPUT_LENGTH; i++)
    {
        ap_test_inst.gpio_read_buff[i - TEST_GPIO_INPUT_0] = gpio_get(i);
    }
}


static void _writeGpio(bool value)
{
    for(int i = TEST_GPIO_OUTPUT_0; i < TEST_GPIO_OUTPUT_LENGTH; i++)
    {
        gpio_put(i, value);
    }
}
