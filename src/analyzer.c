#include "../headers/analyzer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t analyzer_count_CPUs(char* data)
{
    size_t counter = 0;  
    char cpuChecker[4];
    char* lclPointer = strchr(data, '\n') + 1;
    strncpy(cpuChecker,lclPointer,3);

    while(strcmp(cpuChecker,"cpu") == status_succes)
    {
        counter++;
        lclPointer = strchr(lclPointer, '\n') + 1;
        strncpy(cpuChecker,lclPointer,3);
    }

    return counter;
}

extern void* analyzer_task(void *arg)
{
    queue_RA_data** RA_data = (queue_RA_data**)(arg);
    pthread_mutex_t* mutex = (*RA_data)->mutex;

    char* line_beg_ptr = (*RA_data)->data;
    char* line_end_ptr = strchr(line_beg_ptr, '\n') + 1;
    size_t line_len = (size_t)line_end_ptr - (size_t)line_beg_ptr;
    char single_cpu_info[line_len];

    //INITIAL PROCESS
    const size_t cpus_counter = analyzer_count_CPUs((*RA_data)->data);

    size_t user[cpus_counter];
    size_t nice[cpus_counter];
    size_t system[cpus_counter];
    size_t idle[cpus_counter];
    size_t iowait[cpus_counter];
    size_t irq[cpus_counter];
    size_t softirq[cpus_counter];
    size_t steal[cpus_counter];
    
    while(control)
    {
        pthread_mutex_lock(mutex);
        for(size_t i = 0; i < cpus_counter; i++)
        {

            size_t PrevIdle = idle[i] + iowait[i];
            size_t PrevNonIdle = user[i] + nice[i] + system[i] + irq[i] + softirq[i] + steal[i];
            size_t PrevTotal = PrevIdle + PrevNonIdle;
            
            strncpy(single_cpu_info, line_beg_ptr, line_len);
            line_beg_ptr = line_end_ptr;
            line_end_ptr = strchr(line_beg_ptr, '\n') + 1;
            line_len = (size_t)line_end_ptr - (size_t)line_beg_ptr;
            sscanf(single_cpu_info, "%zu %zu %zu %zu %zu %zu %zu %zu", &user[i], &nice[i], &system[i], &idle[i], &iowait[i], &irq[i], &softirq[i], &steal[i]);

            size_t Idle = idle[i] + iowait[i];
            size_t NonIdle = user[i] + nice[i] + system[i] + irq[i] + softirq[i] + steal[i];
            size_t Total = Idle + NonIdle;

            size_t totald = Total - PrevTotal;
            size_t idled = Idle - PrevIdle;

            double CPU_Percentage = ((double)totald - (double)idled)/(double)totald;
            printf("CPU %zu %lf %%", i, CPU_Percentage);
            pthread_mutex_unlock(mutex);
        }
    }



    analyzer_count_CPUs((*RA_data)->data);
    return NULL;
}



void analyzer_end_task(void)
{
    control = control_end;
}

