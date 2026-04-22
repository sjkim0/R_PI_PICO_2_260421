#include "ap_adc.h"
#include "ap_task.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include <stdio.h>

#include "hardware/dma.h"
// For resistor DAC output:
#include "pico/multicore.h"
#include "hardware/pio.h"
#include "resistor_dac.pio.h"


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
        apTaskInit(true, &ap_adc_inst.task[i], 1000, _apAdcHelloTask, true, 0);
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


// ----------------------------------------------------------------------------
// Code for driving the "DAC" output for us to measure

// Core 1 is just going to sit and drive samples out continuously. PIO provides
// consistent sample frequency.

#define OUTPUT_FREQ_KHZ 5
#define SAMPLE_WIDTH 5
// This is the green channel on the VGA board
#define DAC_PIN_BASE 6

void core1_main() {
    PIO pio = pio0;
    uint sm = pio_claim_unused_sm(pio0, true);
    uint offset = pio_add_program(pio0, &resistor_dac_5bit_program);
    resistor_dac_5bit_program_init(pio0, sm, offset,
        OUTPUT_FREQ_KHZ * 1000 * 2 * (1 << SAMPLE_WIDTH), DAC_PIN_BASE);
    while (true) {
        // Triangle wave
        for (int i = 0; i < (1 << SAMPLE_WIDTH); ++i)
            pio_sm_put_blocking(pio, sm, i);
        for (int i = 0; i < (1 << SAMPLE_WIDTH); ++i)
            pio_sm_put_blocking(pio, sm, (1 << SAMPLE_WIDTH) - 1 - i);
    }
}