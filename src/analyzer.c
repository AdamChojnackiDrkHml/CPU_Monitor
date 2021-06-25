#include "../headers/analyzer.h"
#include "../headers/global.h"
#include "../headers/logger.h"
#include "../headers/watchdog.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static volatile size_t analyzer_control = 1;
static size_t analyzer_num_of_data_to_analyze = 8;
static const size_t analyzer_first_scan = 1;
static unsigned short analyzer_num_of_cpus = 0;
static volatile size_t analyzer_end_state = THREAD_WORKING;
static unsigned short analyzer_is_analyzed = FAILURE;
static double* analyzer_analyze_results = NULL;
static size_t** analyzer_data = NULL;
static char* analyzer_local_data = NULL;

static queue_string_data* analyzer_RA_data = NULL;
static pthread_mutex_t* analyzer_RA_mutex = NULL;
static sem_t* analyzer_RA_full = NULL;
static sem_t* analyzer_RA_empty = NULL;
static queue_string_data_record* analyzer_curr_RA_record = NULL;

static queue_number_data* analyzer_AP_data = NULL;
static pthread_mutex_t* analyzer_AP_mutex = NULL;
static sem_t* analyzer_AP_full = NULL;
static sem_t* analyzer_AP_empty = NULL;


static void analyzer_init_data(void);
static void analyzer_count_CPUs(void);
static size_t analyzer_get_data_from_reader(size_t is_frist_scan);
static size_t analyzer_alloc_memory_for_CPU_data(void);
static void analyzer_send_to_printer(void);
static double*  analyzer_interpreter(void);
static void analyzer_free_data(void);

void* analyzer_task(void *arg)
{
    analyzer_init_data();
    logger_log("ANALYZER : Initialized shared data\n");
    watchdog_set_me_alive(Analyzer_ID);

    sem_wait(analyzer_RA_full);
    pthread_mutex_lock(analyzer_RA_mutex);
    if(analyzer_RA_data->status == STATUS_END_BEFORE_WRITE || analyzer_RA_data->status == STATUS_SAFE_END)
    {
        pthread_mutex_unlock(analyzer_RA_mutex);
    }
    else 
    {
        watchdog_set_me_alive(Analyzer_ID);

        analyzer_get_data_from_reader(analyzer_first_scan);
        logger_log("ANALYZER : Recieved first data from reader\n");
        watchdog_set_me_alive(Analyzer_ID);

        analyzer_count_CPUs();
        logger_log("ANALYZER : Counted CPUs\n");
        
        analyzer_alloc_memory_for_CPU_data();
        analyzer_analyze_results = analyzer_interpreter();
        if(analyzer_analyze_results != NULL)
        {
            analyzer_is_analyzed = SUCCESS;
        }
        analyzer_send_to_printer();
        watchdog_set_me_alive(Analyzer_ID);

        analyzer_analyze_results = NULL;
        logger_log("ANALYZER : Send first data to printer\n");
        watchdog_set_me_alive(Analyzer_ID);

        while(analyzer_control && !analyzer_is_analyzed)
        {
            watchdog_set_me_alive(Analyzer_ID);
            if(analyzer_get_data_from_reader(!analyzer_first_scan))
            {
                break;
            }
            logger_log("ANALYZER : Recieved data\n");

            analyzer_analyze_results = analyzer_interpreter();
            if(analyzer_analyze_results == NULL)
            {
                break;
            }
            logger_log("ANALYZER : Analyzed data\n");

            analyzer_send_to_printer();
            logger_log("ANALYZER : Send data to print\n");

            analyzer_analyze_results = NULL;
            watchdog_set_me_alive(Analyzer_ID);
        }
        
    }
    if(analyzer_is_analyzed == FAILURE)
    {
        analyzer_AP_data->status = STATUS_END_BEFORE_WRITE;
    }
    else 
    {
        analyzer_AP_data->status = STATUS_SAFE_END;
    }
    sem_post(analyzer_AP_full);
    while(analyzer_control); 
    analyzer_free_data();
    logger_log("ANALYZER : Freed all recources, exiting\n");
    analyzer_end_state = THREAD_END;
    return NULL;
}

static void analyzer_init_data(void)
{
    analyzer_RA_data = queue_get_RA_data_instance();
    analyzer_RA_mutex = analyzer_RA_data->mutex;
    analyzer_RA_full = analyzer_RA_data->string_data_sem_Full;
    analyzer_RA_empty = analyzer_RA_data->string_data_sem_Empty;

    analyzer_AP_data = queue_get_AP_data_instance();
    analyzer_AP_mutex = analyzer_AP_data->mutex;
    analyzer_AP_full = analyzer_AP_data->number_data_sem_Full;
    analyzer_AP_empty = analyzer_AP_data->number_data_sem_Empty;
}

static void analyzer_count_CPUs(void)
{
    char cpu_format_checker[4] = "err";
    char* curr_line_pointer = strchr(analyzer_local_data, '\n') + 1;
    strncpy(cpu_format_checker,curr_line_pointer,3);

    while(strcmp(cpu_format_checker,"cpu") == SUCCESS)
    {
        analyzer_num_of_cpus++;
        curr_line_pointer = strchr(curr_line_pointer, '\n') + 1;
        strncpy(cpu_format_checker,curr_line_pointer,3);
    }
}

static size_t analyzer_get_data_from_reader(size_t is_first_scan)
{
    if(!is_first_scan)
    {
        sem_wait(analyzer_RA_full);
        pthread_mutex_lock(analyzer_RA_mutex);
    }
    if(!is_first_scan && analyzer_RA_data->status == STATUS_SAFE_END)
    {
        pthread_mutex_unlock(analyzer_RA_mutex);
        return FAILURE;
    }
    analyzer_curr_RA_record = queue_dequeue_RA();
    analyzer_local_data = (char*)calloc(analyzer_curr_RA_record->size * DATA_CHUNK_SIZE + 1,sizeof(char));
    if(analyzer_local_data == NULL)
    {
        free(analyzer_curr_RA_record->string_data);
        analyzer_curr_RA_record->string_data = NULL;
        pthread_mutex_unlock(analyzer_RA_mutex);
        return FAILURE;
    }
    strcpy(analyzer_local_data,analyzer_curr_RA_record->string_data);
    free(analyzer_curr_RA_record->string_data);
    analyzer_curr_RA_record->string_data = NULL;
    pthread_mutex_unlock(analyzer_RA_mutex);
    sem_post(analyzer_RA_empty);
    return SUCCESS;
}

static size_t analyzer_alloc_memory_for_CPU_data(void)
{
    analyzer_data = (size_t**)calloc(analyzer_num_of_data_to_analyze,sizeof(size_t*));
    if(analyzer_data == NULL)
    {
        return FAILURE;
    }
    for(size_t i = 0; i < analyzer_num_of_data_to_analyze; i++)
    {
        analyzer_data[i] = (size_t*)calloc(analyzer_num_of_cpus, sizeof(size_t));
        if(analyzer_data[i] == NULL)
        {
            return FAILURE;
        }
    }
    return SUCCESS;
}

static void analyzer_send_to_printer(void)
{
    sem_wait(analyzer_AP_empty);
    pthread_mutex_lock(analyzer_AP_mutex);
    analyzer_AP_data->num_of_CPUs = analyzer_num_of_cpus;
    queue_enqueue_AP(analyzer_analyze_results);
    analyzer_AP_data->status = STATUS_WORKING;
    pthread_mutex_unlock(analyzer_AP_mutex);
    sem_post(analyzer_AP_full);
}

static double* analyzer_interpreter(void)
{
        double* interpreted_data = ( double*)calloc(analyzer_num_of_cpus, sizeof(double));
        if(interpreted_data == NULL)
        {
            free(analyzer_local_data);
            analyzer_local_data = NULL;
            return NULL;
        }
        char* line_beg_ptr = NULL;
        char* line_end_ptr = NULL;
        line_beg_ptr = analyzer_local_data;
        line_end_ptr = strchr(analyzer_local_data, '\n') + 1;
        line_beg_ptr = analyzer_local_data;
        line_end_ptr = strchr(analyzer_local_data, '\n') + 1;

        for(size_t i = 0; i < analyzer_num_of_cpus; i++)
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
            interpreted_data[i] = CPU_Percentage;
        }
        
        free(analyzer_local_data);
        analyzer_local_data = NULL;
        return interpreted_data;
}

static void analyzer_free_data(void)
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

void analyzer_call_exit(void)
{
    analyzer_control = END_THREAD;
    logger_log("MAIN in ANALYZER :  Recieved signal to end\n");
    while(analyzer_end_state == THREAD_WORKING);
}

