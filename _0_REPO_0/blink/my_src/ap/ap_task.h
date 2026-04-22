#ifndef AP_TASK_H
#define AP_TASK_H


#include "ap_def.h"


typedef struct
{
    bool is_active;
    uint32_t interval_ms;
    uint32_t last_run_time_ms;
    void (*task_func)(void);
    bool run_forever;  // If true, ignore run_count and run indefinitely
    uint32_t run_count;
} ap_task_t;


void apTaskInit(bool is_active, ap_task_t* task, uint32_t interval_ms, void (*task_func)(void), bool run_forever, uint32_t run_count);
void apTaskRun(ap_task_t* task);
void apTaskSetActive(ap_task_t* task, bool is_active);


#endif