#include "ap_test.h"
#include "ap_task.h"


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


void apTestInit(void)
{

}

void apTestLoop(void)
{

}