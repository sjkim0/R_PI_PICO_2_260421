#include "ap_adc.h"
#include "ap_task.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include <stdio.h>


enum ENUM_AP_ADC_TASK
{
    AP_ADC_TASK_HELLO = 0,
    AP_ADC_TASK_LENGTH
};

enum ENUM_AP_ADC_CH
{
    ENUM_AP_ADC_CH_0 = 0,
    ENUM_AP_ADC_CH_1,
    ENUM_AP_ADC_CH_2,
    ENUM_AP_ADC_CH_3,
    ENUM_AP_ADC_CH_TEMP_SENSOR,
    ENUM_AP_ADC_CH_LENGTH
};

typedef struct
{
    /* data */
    ap_task_t task[AP_ADC_TASK_LENGTH];

    uint16_t adc_read_value[ENUM_AP_ADC_CH_LENGTH];
} ap_adc_t;


static ap_adc_t ap_adc_inst;


static void _apAdcHelloTask(void);

void apAdcInit(void)
{
    adc_init();
    for(int i = 0; i < ENUM_AP_ADC_CH_LENGTH - 1; i++)
    {
        adc_gpio_init(ADC_BASE_PIN + i); // ADC0-3 on GPIO26-29
    }

    for(int i = 0; i < AP_ADC_TASK_LENGTH; i++)
    {
        apTaskInit(true, &ap_adc_inst.task[i], 1000, _apAdcHelloTask);
    }
}

void apAdcLoop(void)
{
    for(int i = 0; i < AP_ADC_TASK_LENGTH; i++)
    {
        apTaskRun(&ap_adc_inst.task[i]);
    }
}

static void _apAdcHelloTask(void)
{
    for(int i = 0; i < ENUM_AP_ADC_CH_LENGTH; i++)
    {
        adc_select_input(i);
        ap_adc_inst.adc_read_value[i] = adc_read();
        printf("ADC Value %d: %d\n", i, ap_adc_inst.adc_read_value[i]);
    }
}