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

    while(strcmp(cpuChecker,"cpu") == analyzer_status_succes)
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
    sem_t* RA_Full = (*RA_data)->RA_Full;
    sem_t* RA_Empty = (*RA_data)->RA_Empty;
    //INITIAL PROCESS
   

    size_t* userData = NULL;
    size_t* niceData = NULL;
    size_t* systemData = NULL;
    size_t* idleData = NULL;
    size_t* iowaitData = NULL;
    size_t* irqData = NULL;
    size_t* softirqData = NULL;
    size_t* stealData = NULL;
    size_t cpus_counter = 0;
    char isFirst = 1;
    char* line_beg_ptr = NULL;
    char* line_end_ptr = NULL;
    size_t line_len = 0;

    while(analyzer_control)
    {
        sem_wait(RA_Full);
        pthread_mutex_lock(mutex);
        if(isFirst)
        {
            cpus_counter = analyzer_count_CPUs((*RA_data)->data);
            userData = (size_t*)malloc(sizeof(size_t)* cpus_counter);
            niceData = (size_t*)malloc(sizeof(size_t)* cpus_counter);
            systemData = (size_t*)malloc(sizeof(size_t)* cpus_counter);
            idleData = (size_t*)malloc(sizeof(size_t)* cpus_counter);
            iowaitData = (size_t*)malloc(sizeof(size_t)* cpus_counter);
            irqData = (size_t*)malloc(sizeof(size_t)* cpus_counter);
            softirqData = (size_t*)malloc(sizeof(size_t)* cpus_counter);
            stealData = (size_t*)malloc(sizeof(size_t)* cpus_counter);

            isFirst = 0;
        }
        line_beg_ptr = (*RA_data)->data;
        line_end_ptr = strchr(line_beg_ptr, '\n') + 1;
        line_len = (size_t)line_end_ptr - (size_t)line_beg_ptr;
        line_beg_ptr = line_end_ptr;
        line_end_ptr = strchr(line_beg_ptr, '\n') + 1;
        line_len = (size_t)line_end_ptr - (size_t)line_beg_ptr;
        char single_cpu_info[line_len];
        system("clear");
        for(size_t i = 0; i < cpus_counter; i++)
        {

            size_t PrevIdle = idleData[i] + iowaitData[i];
            size_t PrevNonIdle = userData[i] + niceData[i] + systemData[i] + irqData[i] + softirqData[i] + stealData[i];
            size_t PrevTotal = PrevIdle + PrevNonIdle;
            
            strncpy(single_cpu_info, line_beg_ptr, line_len);
            line_beg_ptr = line_end_ptr;
            line_end_ptr = strchr(line_beg_ptr, '\n') + 1;
            line_len = (size_t)line_end_ptr - (size_t)line_beg_ptr;
            size_t userN, niceN, systemN, idleN, iowaitN, irqN, softirqN, stealN;
            char cpuTrash[4];
            sscanf(single_cpu_info, "%s %zu %zu %zu %zu %zu %zu %zu %zu", cpuTrash, &userN, &niceN, &systemN, &idleN, &iowaitN, &irqN, &softirqN, &stealN);

            userData[i] = userN;
            niceData[i] = niceN;
            systemData[i] = systemN;
            idleData[i] = idleN;
            iowaitData[i] = iowaitN;
            irqData[i] = irqN;
            softirqData[i] = softirqN;
            stealData[i] = stealN;

            size_t Idle = idleData[i] + iowaitData[i];
            size_t NonIdle = userData[i] + niceData[i] + systemData[i] + irqData[i] + softirqData[i] + stealData[i];
            size_t Total = Idle + NonIdle;

            size_t totald = Total - PrevTotal;
            size_t idled = Idle - PrevIdle;

            double divided = (double)totald - (double)idled;
            double divider = (double)totald;
            double CPU_Percentage = divided/divider * 100.0;
            fflush( stdout );
            printf("CPU %zu\t %0.2lf%%\n", i, CPU_Percentage);
        }
        
        free((*RA_data)->data);
        (*RA_data)->data = NULL;
        pthread_mutex_unlock(mutex);
        sem_post(RA_Empty);

    }
    free(userData);
    free(niceData);
    free(systemData);
    free(idleData);
    free(iowaitData);
    free(irqData);
    free(softirqData);
    free(stealData);
    analyzer_count_CPUs((*RA_data)->data);
    return NULL;
}



void analyzer_end_task(void)
{
    analyzer_control = analyzer_control_end;
}

