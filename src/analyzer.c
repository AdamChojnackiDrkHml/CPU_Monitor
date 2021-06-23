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
static double* results = NULL;

static char* analyzer_local_data = NULL;
static unsigned short cpus_counter = 0;

static queue_RA_record* curr_record = NULL;

static unsigned short is_analyzed = 0;

static unsigned short analyzer_count_CPUs()
{
    unsigned short counter = 0;  
    char cpuChecker[4] = "AAA";
    char* lclPointer = strchr(analyzer_local_data, '\n') + 1;
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
            results[i] = CPU_Percentage;
        }
        
        free(analyzer_local_data);
        analyzer_local_data = NULL;

}


void* analyzer_task(void *arg)
{
    queue_RA_data* RA_data = queue_get_RA_data_instance();
    pthread_mutex_t* RA_mutex = RA_data->mutex;
    sem_t* RA_Full = RA_data->RA_Full;
    sem_t* RA_Empty = RA_data->RA_Empty;

    queue_AP_data* AP_data = queue_get_AP_data_instance();
    pthread_mutex_t* AP_mutex = AP_data->mutex;
    sem_t* AP_Full = AP_data->AP_Full;
    sem_t* AP_Empty = AP_data->AP_Empty;
    //INITIAL PROCESS
   


    cpus_counter = 0;


    sem_wait(RA_Full);
    pthread_mutex_lock(RA_mutex);
    if(RA_data->status == 3 || RA_data->status == 0)
    {
        pthread_mutex_unlock(RA_mutex);

    }
    else 
    {
        curr_record = queue_dequeue_RA();
        analyzer_local_data = (char*)calloc(curr_record->size * data_chunk_size + 1,sizeof(char));
        strcpy(analyzer_local_data,curr_record->data);
        free(curr_record->data);
        curr_record->data = NULL;
        
        pthread_mutex_unlock(RA_mutex);
        sem_post(RA_Empty);

        cpus_counter = analyzer_count_CPUs();
        userData = (size_t*)calloc(cpus_counter, sizeof(size_t));
        niceData = (size_t*)calloc(cpus_counter, sizeof(size_t));
        systemData = ( size_t*)calloc(cpus_counter, sizeof(size_t));
        idleData = ( size_t*)calloc(cpus_counter, sizeof(size_t));
        iowaitData = ( size_t*)calloc(cpus_counter, sizeof(size_t));
        irqData = ( size_t*)calloc(cpus_counter, sizeof(size_t));
        softirqData = ( size_t*)calloc(cpus_counter, sizeof(size_t));
        stealData = ( size_t*)calloc(cpus_counter, sizeof(size_t));
        results = ( double*)calloc(cpus_counter, sizeof(double));
        analyzer_interpreter();

        sem_wait(AP_Empty);
        pthread_mutex_lock(AP_mutex);
        AP_data->num_of_CPUs = cpus_counter;
        queue_enqueue_AP(results);

        pthread_mutex_unlock(AP_mutex);
        sem_post(AP_Full);
        is_analyzed = 1;
        if(userData == NULL || niceData == NULL || systemData == NULL || idleData == NULL || iowaitData == NULL || irqData == NULL || softirqData  == NULL || stealData  == NULL)
        {
            fprintf(stderr, "Error, failed allocating memory for arrays, exiting \n");
            return NULL;
        }

        while(analyzer_control)
        {
            sem_wait(RA_Full);
            pthread_mutex_lock(RA_mutex);
            if(RA_data->status == 0)
            {
                pthread_mutex_unlock(RA_mutex);
                break;
            }
            curr_record = queue_dequeue_RA();
            analyzer_local_data = (char*)calloc(curr_record->size * data_chunk_size + 1,sizeof(char));
            strcpy(analyzer_local_data,curr_record->data);
            free(curr_record->data);
            curr_record->data = NULL;
            pthread_mutex_unlock(RA_mutex);
            sem_post(RA_Empty);

            analyzer_interpreter();

            sem_wait(AP_Empty);
            pthread_mutex_lock(AP_mutex);
            
            queue_enqueue_AP(results);
            AP_data->status = 2;
            pthread_mutex_unlock(AP_mutex);
            sem_post(AP_Full);
        
        }

        fflush( stdout );
        printf("IAM OUT");
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
    }
    if(!is_analyzed)
    {
        AP_data->status = 3;
        
    }
    else 
    {
        AP_data->status = 0;
    }
    return NULL;
}



void analyzer_call_exit(void)
{
    analyzer_control = analyzer_control_end;
    while(!end_succes);
}

