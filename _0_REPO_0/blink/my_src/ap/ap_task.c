#include "ap_task.h"


void apTaskInit(ap_task_t* task, uint32_t interval_ms, void (*task_func)(void), bool is_active)
{
    task->interval_ms = interval_ms;
    task->last_run_time_ms = getTickMs();
    task->task_func = task_func;
    task->is_active = is_active;    
}

void apTaskRun(ap_task_t* task)
{
    uint32_t current_time_ms = getTickMs();
    if (task->is_active && current_time_ms - task->last_run_time_ms >= task->interval_ms)
    {
        task->task_func();
        task->last_run_time_ms = current_time_ms;
    }
}

void apTaskSetActive(ap_task_t* task, bool is_active)
{
    task->is_active = is_active;
}