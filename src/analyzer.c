#include "../headers/analyzer.h"
#include "../headers/global.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t analyzer_control = 1;
static const size_t analyzer_control_end = 0;
static volatile size_t end_succes = 0;
static const size_t analyzer_status_succes = 0;
static const size_t analyzer_status_failure = 1;

static size_t* userData = NULL;
static size_t* niceData = NULL;
static size_t* systemData = NULL;
static size_t* idleData = NULL;
static size_t* iowaitData = NULL;
static size_t* irqData = NULL;
static size_t* softirqData = NULL;
static size_t* stealData = NULL;

static char* analyzer_local_data = NULL;
static size_t cpus_counter = 0;
static size_t analyzer_count_CPUs(char* data)
{
    size_t counter = 0;  
    char cpuChecker[4] = "AAA";
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

static void analyzer_interpreter(void)
{

        char* line_beg_ptr = NULL;
        char* line_end_ptr = NULL;
        line_beg_ptr = analyzer_local_data;
        line_end_ptr = strchr(analyzer_local_data, '\n') + 1;
        line_beg_ptr = analyzer_local_data;
        line_end_ptr = strchr(analyzer_local_data, '\n') + 1;

        //system("clear");
        printf("\n\n");
        for(size_t i = 0; i < cpus_counter; i++)
        {
            
            size_t PrevIdle = idleData[i] + iowaitData[i];
            size_t PrevNonIdle = userData[i] + niceData[i] + systemData[i] + irqData[i] + softirqData[i] + stealData[i];
            size_t PrevTotal = PrevIdle + PrevNonIdle;
            
            line_beg_ptr = line_end_ptr;
            line_end_ptr = strchr(line_beg_ptr, '\n');
            *line_end_ptr = '\0';
            line_end_ptr+=1;
            line_beg_ptr +=5;

           

            sscanf(line_beg_ptr, "%zu %zu %zu %zu %zu %zu %zu %zu", userData + i , niceData + i, systemData + i, idleData + i, iowaitData + i, irqData + i, softirqData + i, stealData + i);



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
        
        free(analyzer_local_data);
        analyzer_local_data = NULL;
        printf("%zu\n", analyzer_control);
}


void* analyzer_task(void *arg)
{
    queue_RA_data** RA_data = (queue_RA_data**)(arg);
    pthread_mutex_t* mutex = (*RA_data)->mutex;
    sem_t* RA_Full = (*RA_data)->RA_Full;
    sem_t* RA_Empty = (*RA_data)->RA_Empty;
    //INITIAL PROCESS
   


    cpus_counter = 0;


    sem_wait(RA_Full);
    pthread_mutex_lock(mutex);
    if((*RA_data)->status == 3 || (*RA_data)->status == 0)
    {
        pthread_mutex_unlock(mutex);
        pthread_exit(NULL);
    }
    cpus_counter = analyzer_count_CPUs((*RA_data)->data);
        
    analyzer_local_data = (char*)calloc((*RA_data)->size * data_chunk_size + 1,sizeof(char));
    strcpy(analyzer_local_data,(*RA_data)->data);
    free((*RA_data)->data);
    (*RA_data)->data = NULL;
    pthread_mutex_unlock(mutex);
    sem_post(RA_Empty);

    userData = (size_t*)calloc(cpus_counter, sizeof(size_t));
    niceData = (size_t*)calloc(cpus_counter, sizeof(size_t));
    systemData = ( size_t*)calloc(cpus_counter, sizeof(size_t));
    idleData = ( size_t*)calloc(cpus_counter, sizeof(size_t));
    iowaitData = ( size_t*)calloc(cpus_counter, sizeof(size_t));
    irqData = ( size_t*)calloc(cpus_counter, sizeof(size_t));
    softirqData = ( size_t*)calloc(cpus_counter, sizeof(size_t));
    stealData = ( size_t*)calloc(cpus_counter, sizeof(size_t));

    analyzer_interpreter();
    if(userData == NULL || niceData == NULL || systemData == NULL || idleData == NULL || iowaitData == NULL || irqData == NULL || softirqData  == NULL || stealData  == NULL)
    {
        fprintf(stderr, "Error, failed allocating memory for arrays, exiting \n");
        return NULL;
    }

    while(analyzer_control)
    {
        sem_wait(RA_Full);
        pthread_mutex_lock(mutex);
        if((*RA_data)->status == 0)
        {
            pthread_mutex_unlock(mutex);
            break;
        }
        analyzer_local_data = (char*)calloc((*RA_data)->size * data_chunk_size + 1,sizeof(char));
        strcpy(analyzer_local_data,(*RA_data)->data);
        free((*RA_data)->data);
        (*RA_data)->data = NULL;
        pthread_mutex_unlock(mutex);
        sem_post(RA_Empty);

        analyzer_interpreter();

    }
    fflush( stdout );
    printf("IAM OUT");
    end_succes = 1;
    free(userData);
    free(niceData);
    free(systemData);
    free(idleData);
    free(iowaitData);
    free(irqData);
    free(softirqData);
    free(stealData);
    userData = NULL;
    niceData = NULL;
    systemData = NULL;
    idleData = NULL;
    iowaitData = NULL;
    irqData = NULL;
    softirqData = NULL;
    stealData = NULL;
    printf("All Freed");
    end_succes = 1;
    pthread_exit(NULL);
}



void analyzer_call_exit(void)
{
    analyzer_control = analyzer_control_end;
    while(!end_succes);
}

