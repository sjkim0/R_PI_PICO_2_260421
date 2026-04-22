#include "ap_test.h"
#include "ap_task.h"

// Pico W devices use a GPIO on the WIFI chip for the LED,
// so when building for Pico W, CYW43_WL_GPIO_LED_PIN will be defined
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif
#ifndef LED_DELAY_MS
#define LED_DELAY_MS 250
#endif

enum ENUM_AP_ADC_TASK
{
    AP_TEST_TASK_0 = 0,
    AP_TEST_TASK_LENGTH
};

typedef struct
{
    /* data */
    ap_task_t task[AP_TEST_TASK_LENGTH];

} ap_test_t;


static ap_test_t ap_test_inst;


// Perform initialisation
static int pico_led_init(void);

// Turn the led on or off
static void pico_set_led(bool led_on);


void apTestInit(void)
{
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);
}

void apTestLoop(void)
{
    pico_set_led(true);
    sleep_ms(LED_DELAY_MS);
    pico_set_led(false);
    sleep_ms(LED_DELAY_MS);
}

static void pico_set_led(bool led_on) 
{
#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}

static int pico_led_init(void)
{
#if defined(PICO_DEFAULT_LED_PIN)
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // For Pico W devices we need to initialise the driver etc
    return cyw43_arch_init();
#endif
}