#include "../headers/logger.h"
#include "../headers/queue.h"
#include "../headers/global.h"
#include "../headers/watchdog.h"
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>

static volatile size_t logger_control = 1;
static const size_t logger_control_end = 0;

static volatile size_t end_succes = 0;

static queue_RA_record* current_record = NULL;
static char* logger_local_data = NULL;
static volatile _Atomic(char) data_flag = 0;
static FILE * fd;

static queue_RA_data* log_data = NULL;
static pthread_mutex_t* mutex =  NULL;
static sem_t* sem_Full =  NULL;
static sem_t* sem_Empty =  NULL;

static void logger_init_shared_data(void)
{
    log_data = queue_get_log_data_instance();
    mutex = log_data->mutex;
    sem_Full = log_data->RA_Full;
    sem_Empty = log_data->RA_Empty;
    data_flag = 1;
}


void* logger_task(void * arg)
{   
    fd = fopen("../logs.txt", "a+");
    logger_init_shared_data();
    while(logger_control)
    {
        sem_wait(sem_Full);
        if(log_data->status == 0)
        {
            break;
        }
        pthread_mutex_lock(mutex);
        current_record = queue_dequeue_log();
        logger_local_data = (char*)calloc(max_log_size,sizeof(char));
        strcpy(logger_local_data,current_record->data);
        free(current_record->data);
        current_record->data = NULL;
        pthread_mutex_unlock(mutex);
        sem_post(sem_Empty);
        fflush(fd);
        fputs(logger_local_data, fd );
        free(logger_local_data);
        logger_local_data = NULL;
        watchdog_set_me_alive(Logger_ID);
    }

    fclose(fd);
    end_succes = 1;
    return NULL;
}

void logger_call_exit(void)
{
    logger_control = logger_control_end;
    if(data_flag)
    {
        log_data->status = 0;
        sem_post(sem_Full); 
    }
    while(!end_succes);
}


void logger_log(char* log)
{
    if(!data_flag)
    {
        return;
    }
    char* to_log = (char*)calloc(max_log_size, sizeof(char));
    strcpy(to_log, log);
    sem_wait(sem_Empty);
    pthread_mutex_lock(mutex);
    queue_enqueue_log(to_log, max_log_size);
    log_data->status = 2;
    pthread_mutex_unlock(mutex);
    sem_post(sem_Full);
}
