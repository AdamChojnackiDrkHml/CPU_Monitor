#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../headers/queue.h"
#include "../headers/global.h"

static queue_RA_data* RA_data = NULL;
static queue_RA_data* log_data = NULL;
static queue_AP_data* AP_data = NULL;

static void queue_create_string_data(pthread_mutex_t* mutex, sem_t* sem_Full, sem_t* sem_Empty, size_t isLogger)
{
    queue_RA_data* newData = (queue_RA_data*)calloc(1, sizeof(queue_RA_data));
    unsigned short queue_size = isLogger ? logger_queue_size : default_queue_size;
    if(newData == NULL)
    {
        fprintf(stderr, "Error allocating memory for queue_RA_data struct, exiting\n");
        return;
    }
    queue_RA_record** arr = (queue_RA_record**)calloc(queue_size, sizeof(queue_RA_record*));
    if(arr == NULL)
    {
        fprintf(stderr, "Error allocating memory for circular queue array, exiting\n");
        return;
    }

    for(size_t i = 0; i < queue_size; i++)
    {
        queue_RA_record* new_record = (queue_RA_record*)calloc(1, sizeof(queue_RA_record));
        if(arr == NULL)
        {
            fprintf(stderr, "Error allocating memory for circular queue record, exiting\n");
            return;
        }
        arr[i] = new_record;
    }

    newData->status = 0;
    newData->mutex = mutex;
    newData->RA_Empty = sem_Empty;
    newData->RA_Full = sem_Full;
    newData->data = arr;

    if(isLogger)
    {
       log_data = newData;
    }
    else
    {
        RA_data = newData;
    }
}

static void queue_destroy_string_data(size_t isLogger)
{
    queue_RA_data* to_destroy = isLogger ? log_data : RA_data;
    unsigned short queue_size = isLogger ? logger_queue_size : default_queue_size;
    if(to_destroy == NULL)
    {
        return;
    }
    for(size_t i = 0; i < queue_size; i++)
    {
        if(to_destroy->data[i] != NULL)
        {
            if(to_destroy->data[i]->data != NULL)
            {
                free(to_destroy->data[i]->data);
                to_destroy->data[i]->data = NULL;
            }
            free(to_destroy->data[i]);
            to_destroy->data[i] = NULL;
        }
    }
    if(to_destroy->data != NULL)
    {
        free(to_destroy->data);
        to_destroy->data = NULL;
    }

    free(to_destroy);
    to_destroy = NULL;

    if(isLogger)
    {
        log_data = NULL;
    }
    else
    {
        RA_data = NULL;
    }
}

static void queue_enqueue_string(char* data, unsigned short size, size_t isLogger)
{
    if(isLogger)
    {
        unsigned short new_index = log_data->in++ & (logger_queue_size-1);
        log_data->data[new_index]->data = data;
        log_data->data[new_index]->size = size;
    }
    else
    {
        unsigned short new_index = RA_data->in++ & (default_queue_size-1);
        RA_data->data[new_index]->data = data;
        RA_data->data[new_index]->size = size;
    }

}

static queue_RA_record* queue_dequeue_string(size_t isLogger)
{
    if (isLogger)
    {
        unsigned short out_index = log_data->out++ & (logger_queue_size-1);
        queue_RA_record* ret = log_data->data[out_index];
        return ret;
    }
    else
    {
        return RA_data->data[RA_data->out++ & (default_queue_size-1)];
    }
}

static queue_RA_data* queue_get_string_data_instance(size_t isLogger)
{
    return isLogger ? log_data : RA_data;
}

static size_t queue_is_string_data_null(size_t isLogger)
{
    queue_RA_data* to_check = isLogger ? log_data : RA_data;
    if(to_check == NULL)
    {
        return 1;
    }
    return 0;
}

void queue_create_RA_data(pthread_mutex_t* mutex, sem_t* sem_Full, sem_t* sem_Empty)
{
    queue_create_string_data(mutex, sem_Full, sem_Empty, 0);
}

void queue_create_log_data(pthread_mutex_t* mutex, sem_t* sem_Full, sem_t* sem_Empty)
{
    queue_create_string_data(mutex, sem_Full, sem_Empty, 1);
}


void queue_destroy_RA_data(void)
{
    queue_destroy_string_data(0);
}

void queue_destroy_log_data(void)
{
    queue_destroy_string_data(1);
}


void queue_enqueue_RA(char* data, unsigned short size)
{
    queue_enqueue_string(data, size, 0);
}

void queue_enqueue_log(char* data, unsigned short size)
{
    queue_enqueue_string(data, size, 1);
}


queue_RA_record* queue_dequeue_RA(void)
{
    return queue_dequeue_string(0);
}

queue_RA_record* queue_dequeue_log(void)
{
    queue_RA_record* ret = queue_dequeue_string(1);
    return ret;
}


queue_RA_data* queue_get_RA_data_instance()
{
    return queue_get_string_data_instance(0);
}

queue_RA_data* queue_get_log_data_instance()
{
    return queue_get_string_data_instance(1);
}

size_t queue_is_RA_data_null()
{
    return queue_is_string_data_null(0);
}

size_t queue_is_log_data_null()
{
    return queue_is_string_data_null(1);
}

void queue_create_AP_data(pthread_mutex_t* mutex, sem_t* AP_Full, sem_t* AP_Empty)
{
    queue_AP_data* newData = (queue_AP_data*)calloc(1,sizeof(queue_AP_data));
    if(newData == NULL)
    {
        fprintf(stderr, "Error allocating memory for queue_AP_data struct, exiting\n");
        return;
    }
    queue_AP_record** arr = (queue_AP_record**)calloc(default_queue_size, sizeof(queue_AP_record*));
    if(arr == NULL)
    {
        fprintf(stderr, "Error allocating memory for circular queue array, exiting\n");
        return;
    }
    for(size_t i = 0; i < default_queue_size; i++)
    {
        queue_AP_record* new_record = (queue_AP_record*)calloc(1, sizeof(queue_AP_record));
        if(arr == NULL)
        {
            fprintf(stderr, "Error allocating memory for circular queue record, exiting\n");
            return;
        }
        arr[i] = new_record;
    }

    newData->status = 0;
    newData->mutex = mutex;
    newData->AP_Empty = AP_Empty;
    newData->AP_Full = AP_Full;
    newData->data = arr;

    AP_data = newData;
    return;
}

void queue_destroy_AP_data(void)
{
    if(AP_data == NULL)
    {
        return;
    }
    for(size_t i = 0; i < default_queue_size; i++)
    {
        if(AP_data->data[i] != NULL)
        {
            if(AP_data->data[i]->data != NULL)
            {
                free(AP_data->data[i]->data);
                AP_data->data[i]->data = NULL;
            }
            free(AP_data->data[i]);
            AP_data->data[i] = NULL;
        }
    }
    if(AP_data->data != NULL)
    {
        free(AP_data->data);
        AP_data->data = NULL;
    }

    free(AP_data);
    AP_data = NULL;
}

void queue_enqueue_AP(double* data)
{
    unsigned short new_index = AP_data->in++ & (default_queue_size-1);
    AP_data->data[new_index]->data = data;
}

queue_AP_record* queue_dequeue_AP(void)
{
    unsigned short out_index = AP_data->out++ & (default_queue_size-1);
    queue_AP_record* ret  = AP_data->data[out_index];
    return ret;
}

queue_AP_data* queue_get_AP_data_instance(void)
{
    return AP_data;
}

size_t queue_is_AP_data_null(void)
{
    if(AP_data == NULL)
    {
        return 1;
    }
    return 0;
}

