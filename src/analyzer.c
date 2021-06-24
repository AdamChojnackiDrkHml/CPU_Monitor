#include "../headers/analyzer.h"
#include "../headers/global.h"
#include "../headers/logger.h"
#include "../headers/watchdog.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t analyzer_control = 1;
static volatile size_t end_state = THREAD_WORKING;
static size_t analyzer_num_of_data_to_analyze = 8;
static size_t analyzer_first_scan = 1;
static unsigned short cpus_counter = 0;
static unsigned short is_analyzed = FAILURE;
static double* results = NULL;
static size_t** analyzer_data = NULL;
static char* analyzer_local_data = NULL;

static queue_string_data* RA_data = NULL;
static pthread_mutex_t* RA_mutex = NULL;
static sem_t* RA_Full = NULL;
static sem_t* RA_Empty = NULL;

static queue_number_data* AP_data = NULL;
static pthread_mutex_t* AP_mutex = NULL;
static sem_t* AP_Full = NULL;
static sem_t* AP_Empty = NULL;

static queue_string_data_record* curr_record = NULL;



static void analyzer_init_data(void);
static unsigned short analyzer_count_CPUs(void);
static double*  analyzer_interpreter(void);
static size_t analyzer_alloc_memory_for_CPU_data(void);
static void analyzer_send_to_printer(void);
static size_t analyzer_get_data_from_reader(size_t is_frist_scan);
static void analyzer_free_data(void);

void* analyzer_task(void *arg)
{
    analyzer_init_data();
   
    logger_log("ANALYZER : Initialized shared data\n");


    watchdog_set_me_alive(Analyzer_ID);
    sem_wait(RA_Full);
    pthread_mutex_lock(RA_mutex);
    if(RA_data->status == STATUS_END_BEFORE_WRITE || RA_data->status == STATUS_SAFE_END)
    {
        pthread_mutex_unlock(RA_mutex);
    }
    else 
    {
        watchdog_set_me_alive(Analyzer_ID);

        analyzer_get_data_from_reader(analyzer_first_scan);

        watchdog_set_me_alive(Analyzer_ID);

        cpus_counter = analyzer_count_CPUs();
        analyzer_alloc_memory_for_CPU_data();
        results = analyzer_interpreter();
        analyzer_send_to_printer();
        watchdog_set_me_alive(Analyzer_ID);

        results = NULL;
        logger_log("ANALYZER : Initialized working data\n");
        watchdog_set_me_alive(Analyzer_ID);



        is_analyzed = SUCCESS;

        while(analyzer_control)
        {
            watchdog_set_me_alive(Analyzer_ID);
            if(analyzer_get_data_from_reader(!analyzer_first_scan))
            {
                break;
            }
            logger_log("ANALYZER : Recieved data\n");

            results = analyzer_interpreter();
            logger_log("ANALYZER : Analyzed data\n");

            analyzer_send_to_printer();
            logger_log("ANALYZER : Send data to print\n");

            results = NULL;
            watchdog_set_me_alive(Analyzer_ID);
        }
        while(analyzer_control);
        analyzer_free_data();
    }
    if(is_analyzed == FAILURE)
    {
        AP_data->status = STATUS_END_BEFORE_WRITE;
        write(0,"a",1);
    }
    else 
    {
        AP_data->status = STATUS_SAFE_END;
        write(0,"b",1);
    }
    sem_post(AP_Full);
    logger_log("ANALYZER : Freed all recources, exiting\n");
    end_state = THREAD_END;
    return NULL;
}

void analyzer_init_data(void)
{
    RA_data = queue_get_RA_data_instance();
    RA_mutex = RA_data->mutex;
    RA_Full = RA_data->string_data_sem_Full;
    RA_Empty = RA_data->string_data_sem_Empty;

    AP_data = queue_get_AP_data_instance();
    AP_mutex = AP_data->mutex;
    AP_Full = AP_data->number_data_sem_Full;
    AP_Empty = AP_data->number_data_sem_Empty;
}

void analyzer_send_to_printer(void)
{
    sem_wait(AP_Empty);
    pthread_mutex_lock(AP_mutex);
    AP_data->num_of_CPUs = cpus_counter;
    queue_enqueue_AP(results);
    AP_data->status = STATUS_WORKING;
    pthread_mutex_unlock(AP_mutex);
    sem_post(AP_Full);
}

size_t analyzer_alloc_memory_for_CPU_data(void)
{
    analyzer_data = (size_t**)calloc(analyzer_num_of_data_to_analyze,sizeof(size_t*));
    if(analyzer_data == NULL)
    {
        return FAILURE;
    }
    for(size_t i = 0; i < analyzer_num_of_data_to_analyze; i++)
    {
        analyzer_data[i] = (size_t*)calloc(cpus_counter, sizeof(size_t));
        if(analyzer_data[i] == NULL)
        {
            return FAILURE;
        }
    }
    return SUCCESS;
}

void analyzer_free_data(void)
{
    if(analyzer_data == NULL)
    {
        return;
    }
    for(size_t i = 0; i < analyzer_num_of_data_to_analyze; i++)
    {
        if(analyzer_data[i] != NULL)
        {
            free(analyzer_data[i]);
            analyzer_data[i] = NULL;
        }
    }
    
    free(analyzer_data);
    analyzer_data = NULL;
    
}

size_t analyzer_get_data_from_reader(size_t is_first_scan)
{
    if(!is_first_scan)
    {
        sem_wait(RA_Full);
        pthread_mutex_lock(RA_mutex);
    }
    if(!is_first_scan && RA_data->status == STATUS_SAFE_END)
    {
        pthread_mutex_unlock(RA_mutex);
        return FAILURE;
    }
    curr_record = queue_dequeue_RA();
    analyzer_local_data = (char*)calloc(curr_record->size * DATA_CHUNK_SIZE + 1,sizeof(char));
    strcpy(analyzer_local_data,curr_record->string_data);
    free(curr_record->string_data);
    curr_record->string_data = NULL;
    pthread_mutex_unlock(RA_mutex);
    sem_post(RA_Empty);
    return SUCCESS;
}

unsigned short analyzer_count_CPUs(void)
{
    unsigned short counter = 0;  
    char cpuChecker[4] = "AAA";
    char* lclPointer = strchr(analyzer_local_data, '\n') + 1;
    strncpy(cpuChecker,lclPointer,3);

    while(strcmp(cpuChecker,"cpu") == SUCCESS)
    {
        counter++;
        lclPointer = strchr(lclPointer, '\n') + 1;
        strncpy(cpuChecker,lclPointer,3);
    }

    return counter;
}

double*  analyzer_interpreter(void)
{
        double * interpreted = ( double*)calloc(cpus_counter, sizeof(double));
        char* line_beg_ptr = NULL;
        char* line_end_ptr = NULL;
        line_beg_ptr = analyzer_local_data;
        line_end_ptr = strchr(analyzer_local_data, '\n') + 1;
        line_beg_ptr = analyzer_local_data;
        line_end_ptr = strchr(analyzer_local_data, '\n') + 1;

        //system("clear");
        for(size_t i = 0; i < cpus_counter; i++)
        {
            
            size_t PrevIdle = analyzer_data[3][i] + analyzer_data[4][i];
            size_t PrevNonIdle = analyzer_data[0][i] + analyzer_data[1][i] + analyzer_data[2][i] + analyzer_data[5][i] + analyzer_data[6][i] + analyzer_data[7][i];
            size_t PrevTotal = PrevIdle + PrevNonIdle;
            
            line_beg_ptr = line_end_ptr;
            line_end_ptr = strchr(line_beg_ptr, '\n');
            *line_end_ptr = '\0';
            line_end_ptr+=1;
            line_beg_ptr +=5;

            sscanf(line_beg_ptr, "%zu %zu %zu %zu %zu %zu %zu %zu", analyzer_data[0] + i, analyzer_data[1] + i, analyzer_data[2] + i,analyzer_data[3] + i, analyzer_data[4] + i, analyzer_data[5] + i, analyzer_data[6] + i, analyzer_data[7] + i);

            size_t Idle = analyzer_data[3][i] + analyzer_data[4][i];
            size_t NonIdle = analyzer_data[0][i] + analyzer_data[1][i] + analyzer_data[2][i] + analyzer_data[5][i] + analyzer_data[6][i] + analyzer_data[7][i];
            size_t Total = Idle + NonIdle;

            size_t totald = Total - PrevTotal;
            size_t idled = Idle - PrevIdle;

            double divided = (double)totald - (double)idled;
            double divider = (double)totald;
            double CPU_Percentage = divided/divider * 100.0;
            interpreted[i] = CPU_Percentage;
        }
        
        free(analyzer_local_data);
        analyzer_local_data = NULL;
        return interpreted;
}


void analyzer_call_exit(void)
{
    analyzer_control = END_THREAD;
    while(end_state == THREAD_WORKING);
}

