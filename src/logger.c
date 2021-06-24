#include "../headers/logger.h"
#include "../headers/queue.h"
#include "../headers/global.h"
#include "../headers/watchdog.h"
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>

//Logger working variables
static volatile size_t end_state = THREAD_WORKING;
static volatile _Atomic(char) data_flag = 0;
static volatile size_t logger_control = 1;
static const size_t max_fopen_tries = 5;
static size_t logger_file_open_flag = FAILURE;
static char* logger_local_data = NULL;
static FILE * fd;

//Shared data and synchronization structures
static queue_string_data* log_data = NULL;
static pthread_mutex_t* mutex =  NULL;
static sem_t* sem_Full =  NULL;
static sem_t* sem_Empty =  NULL;
static queue_string_data_record* current_record = NULL;

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

        fflush(fd);
        fputs(logger_local_data, fd );

        if(logger_reset_data())
        {
            break;
        }
        watchdog_set_me_alive(Logger_ID);
    }
    logger_reset_data();
    while(logger_control);
    end_state = THREAD_END;
    return NULL;
}

size_t logger_recieve_data(void)
{
    sem_wait(sem_Full);
    pthread_mutex_lock(mutex);
    if(log_data->status == STATUS_SAFE_END)
    {
        pthread_mutex_unlock(mutex);
        return FAILURE;
    }
    current_record = queue_dequeue_log();
    logger_local_data = (char*)calloc(MAX_LOG_SIZE,sizeof(char));
    if(logger_local_data == NULL)
    {
        free(current_record->string_data);
        current_record->string_data = NULL;
        pthread_mutex_unlock(mutex);
        return FAILURE;
    }
    strcpy(logger_local_data,current_record->string_data);
    free(current_record->string_data);
    current_record->string_data = NULL;
    pthread_mutex_unlock(mutex);
    sem_post(sem_Empty);

    return SUCCESS;
}

static void logger_init_shared_data(void)
{
    log_data = queue_get_log_data_instance();
    mutex = log_data->mutex;
    sem_Full = log_data->string_data_sem_Full;
    sem_Empty = log_data->string_data_sem_Empty;
    data_flag = 1;
}

static size_t logger_open_file(void)
{

    for(size_t i = 1; i <= max_fopen_tries; i++)
    {
        fd = fopen("../logs.txt", "a+");
        if (fd != NULL)
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
    if(fclose(fd) == EOF)
    {
        return FAILURE;
    }
    logger_file_open_flag = FAILURE;
    fd = NULL;
    return SUCCESS;
}

static size_t logger_reset_data(void)
{
    free(logger_local_data);
    logger_local_data = NULL;
    return logger_close_file();
}

void logger_call_exit(void)
{
    logger_control = END_THREAD;
    if(data_flag)
    {
        log_data->status = STATUS_SAFE_END;
        sem_post(sem_Full); 
    }
    while(end_state == THREAD_WORKING);
}



void logger_log(char* log)
{
    if(!data_flag)
    {
        return;
    }
    char* to_log = (char*)calloc(MAX_LOG_SIZE, sizeof(char));
    if(to_log == NULL)
    {
        return;
    }
    strcpy(to_log, log);
    sem_wait(sem_Empty);
    pthread_mutex_lock(mutex);
    queue_enqueue_log(to_log, MAX_LOG_SIZE);
    log_data->status = STATUS_WORKING;
    pthread_mutex_unlock(mutex);
    sem_post(sem_Full);
}
