#include "../headers/logger.h"
#include "../headers/queue.h"
#include "../headers/global.h"
#include "../headers/watchdog.h"
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>

//Logger working variables
static volatile _Atomic(char) logger_data_flag = 0;
static volatile size_t logger_control = 1;
static const size_t logger_max_fopen_tries = 5;
static volatile size_t logger_end_state = THREAD_WORKING;
static size_t logger_file_open_flag = FAILURE;
static char* logger_local_data = NULL;
static FILE * logger_fp;

//Shared data and synchronization structures
static queue_string_data* logger_log_data = NULL;
static pthread_mutex_t* logger_log_mutex =  NULL;
static sem_t* logger_log_sem_full =  NULL;
static sem_t* logger_log_sem_empty =  NULL;
static queue_string_data_record* logger_curr_record = NULL;

static void logger_init_shared_data(void);
static size_t logger_open_file(void);
static size_t logger_close_file(void);
static size_t logger_reset_data(void);
static size_t logger_recieve_data(void);

void* logger_task(void * arg)
{   
    logger_init_shared_data();
    while(logger_control)
    {
        if(logger_open_file())
        {
            break;
        }

        if(logger_recieve_data())
        {
            break;
        }

        fflush(logger_fp);
        fputs(logger_local_data, logger_fp );

        if(logger_reset_data())
        {
            break;
        }
        watchdog_set_me_alive(Logger_ID);
    }
    logger_reset_data();
    while(logger_control);
    logger_end_state = THREAD_END;
    return NULL;
}

void logger_log(char* log)
{
    if(!logger_data_flag)
    {
        return;
    }
    char* to_log = (char*)calloc(MAX_LOG_SIZE, sizeof(char));
    if(to_log == NULL)
    {
        return;
    }
    strcpy(to_log, log);
    sem_wait(logger_log_sem_empty);
    pthread_mutex_lock(logger_log_mutex);
    queue_enqueue_log(to_log, MAX_LOG_SIZE);
    logger_log_data->status = STATUS_WORKING;
    pthread_mutex_unlock(logger_log_mutex);
    sem_post(logger_log_sem_full);
}

static void logger_init_shared_data(void)
{
    logger_log_data = queue_get_log_data_instance();
    logger_log_mutex = logger_log_data->mutex;
    logger_log_sem_full = logger_log_data->string_data_sem_Full;
    logger_log_sem_empty = logger_log_data->string_data_sem_Empty;
    logger_data_flag = 1;
}

static size_t logger_open_file(void)
{

    for(size_t i = 1; i <= logger_max_fopen_tries; i++)
    {
        logger_fp = fopen("../logs.txt", "a+");
        if (logger_fp != NULL)
        {
            logger_file_open_flag = SUCCESS;
            return SUCCESS;
        }
    }
    printf("LOGGER : Fail to open file 5 times, exit");
    return FAILURE;
}

static size_t logger_close_file(void)
{
    
    if(logger_file_open_flag)
    {
        return SUCCESS;
    }
    if(fclose(logger_fp) == EOF)
    {
        return FAILURE;
    }
    logger_file_open_flag = FAILURE;
    logger_fp = NULL;
    return SUCCESS;
}

static size_t logger_reset_data(void)
{
    free(logger_local_data);
    logger_local_data = NULL;
    return logger_close_file();
}

static size_t logger_recieve_data(void)
{
    watchdog_set_me_alive(Logger_ID);

    sem_wait(logger_log_sem_full);
    pthread_mutex_lock(logger_log_mutex);
    if(logger_log_data->status == STATUS_SAFE_END)
    {
        pthread_mutex_unlock(logger_log_mutex);
        return FAILURE;
    }
    logger_curr_record = queue_dequeue_log();
    logger_local_data = (char*)calloc(MAX_LOG_SIZE,sizeof(char));
    if(logger_local_data == NULL)
    {
        free(logger_curr_record->string_data);
        logger_curr_record->string_data = NULL;
        pthread_mutex_unlock(logger_log_mutex);
        return FAILURE;
    }
    strcpy(logger_local_data,logger_curr_record->string_data);
    free(logger_curr_record->string_data);
    logger_curr_record->string_data = NULL;
    pthread_mutex_unlock(logger_log_mutex);
    sem_post(logger_log_sem_empty);

    return SUCCESS;
}

void logger_call_exit(void)
{
    logger_control = END_THREAD;
    if(logger_data_flag)
    {
        logger_log_data->status = STATUS_SAFE_END;
        sem_post(logger_log_sem_full); 
    }
    while(logger_end_state == THREAD_WORKING);
}


