#include "ap_task.h"

void apTaskInit(bool is_active, ap_task_t* task, uint32_t interval_ms, void (*task_func)(void), bool run_forever, uint32_t run_count)
{
    task->is_active = is_active;    
    task->interval_ms = interval_ms;
    task->last_run_time_ms = getTickMs();
    task->task_func = task_func;
    task->run_forever = run_forever;
    task->run_count = run_count;
}

void apTaskRun(ap_task_t* task)
{
    uint32_t current_time_ms = getTickMs();
    
    if ((task->is_active) && (current_time_ms - task->last_run_time_ms >= task->interval_ms)
                          && (task->run_count > 0 || task->run_forever))
    {
        task->task_func();
        task->last_run_time_ms = current_time_ms;
        if (!task->run_forever)
        {
            task->run_count--;
        }
    }
}

void apTaskSetActive(ap_task_t* task, bool is_active)
{
    task->is_active = is_active;
}