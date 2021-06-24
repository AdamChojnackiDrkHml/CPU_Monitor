#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../headers/queue.h"
#include "../headers/global.h"
#define LOGGER 1
#define RA 0

static queue_string_data* RA_data = NULL;
static queue_string_data* log_data = NULL;
static queue_number_data* AP_data = NULL;

static void queue_create_string_data(pthread_mutex_t* mutex, sem_t* sem_Full, sem_t* sem_Empty, size_t isLogger);
static void queue_destroy_string_data(size_t isLogger);
static void queue_enqueue_string(char* data, unsigned short size, size_t isLogger);
static queue_string_data_record* queue_dequeue_string(size_t isLogger);
static queue_string_data* queue_get_string_data_instance(size_t isLogger);
static size_t queue_is_string_data_null(size_t isLogger);
static void queue_create_RA_data(pthread_mutex_t* mutex, sem_t* sem_Full, sem_t* sem_Empty);
static void queue_create_log_data(pthread_mutex_t* mutex, sem_t* sem_Full, sem_t* sem_Empty);
static void queue_destroy_RA_data(void);
static void queue_destroy_log_data(void);
static size_t queue_is_RA_data_null();
static size_t queue_is_log_data_null();
static void queue_create_AP_data(pthread_mutex_t* mutex, sem_t* AP_Full, sem_t* AP_Empty);
static void queue_destroy_AP_data(void);
static size_t queue_is_AP_data_null(void);

static void (*queues_constructors[NUMBER_OF_SHARED_DATA_OBJECTS])(pthread_mutex_t* mutex, sem_t* sem_full, sem_t* sem_empty) = { queue_create_RA_data,queue_create_AP_data,queue_create_log_data};
static void (*queues_destructors[NUMBER_OF_SHARED_DATA_OBJECTS])(void) = { queue_destroy_RA_data,queue_destroy_AP_data,queue_destroy_log_data};

static void queue_create_string_data(pthread_mutex_t* mutex, sem_t* sem_Full, sem_t* sem_Empty, size_t isLogger)
{
    queue_string_data* newData = (queue_string_data*)calloc(1, sizeof(queue_string_data));
    unsigned short queue_size = isLogger ? logger_queue_size : default_queue_size;
    if(newData == NULL)
    {
        fprintf(stderr, "Error allocating memory for queue_RA_data struct, exiting\n");
        return;
    }
    queue_string_data_record** arr = (queue_string_data_record**)calloc(queue_size, sizeof(queue_string_data_record*));
    if(arr == NULL)
    {
        fprintf(stderr, "Error allocating memory for circular queue array, exiting\n");
        return;
    }

    for(size_t i = 0; i < queue_size; i++)
    {
        queue_string_data_record* new_record = (queue_string_data_record*)calloc(1, sizeof(queue_string_data_record));
        if(arr == NULL)
        {
            fprintf(stderr, "Error allocating memory for circular queue record, exiting\n");
            return;
        }
        arr[i] = new_record;
    }

    newData->status = STATUS_NULL;
    newData->mutex = mutex;
    newData->string_data_sem_Empty = sem_Empty;
    newData->string_data_sem_Full = sem_Full;
    newData->string_data_arr = arr;

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
    queue_string_data* to_destroy = isLogger ? log_data : RA_data;
    unsigned short queue_size = isLogger ? logger_queue_size : default_queue_size;
    if(to_destroy == NULL)
    {
        return;
    }
    for(size_t i = 0; i < queue_size; i++)
    {
        if(to_destroy->string_data_arr[i] != NULL)
        {
            if(to_destroy->string_data_arr[i]->string_data != NULL)
            {
                free(to_destroy->string_data_arr[i]->string_data);
                to_destroy->string_data_arr[i]->string_data = NULL;
            }
            free(to_destroy->string_data_arr[i]);
            to_destroy->string_data_arr[i] = NULL;
        }
    }
    if(to_destroy->string_data_arr != NULL)
    {
        free(to_destroy->string_data_arr);
        to_destroy->string_data_arr = NULL;
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
        log_data->string_data_arr[new_index]->string_data = data;
        log_data->string_data_arr[new_index]->size = size;
    }
    else
    {
        unsigned short new_index = RA_data->in++ & (default_queue_size-1);
        RA_data->string_data_arr[new_index]->string_data = data;
        RA_data->string_data_arr[new_index]->size = size;
    }

}

static queue_string_data_record* queue_dequeue_string(size_t isLogger)
{
    if (isLogger)
    {
        unsigned short out_index = log_data->out++ & (logger_queue_size-1);
        queue_string_data_record* ret = log_data->string_data_arr[out_index];
        return ret;
    }
    else
    {
        return RA_data->string_data_arr[RA_data->out++ & (default_queue_size-1)];
    }
}

static queue_string_data* queue_get_string_data_instance(size_t isLogger)
{
    return isLogger ? log_data : RA_data;
}

static size_t queue_is_string_data_null(size_t isLogger)
{
    queue_string_data* to_check = isLogger ? log_data : RA_data;
    if(to_check == NULL)
    {
        return failure;
    }
    return success;
}

void queue_create_RA_data(pthread_mutex_t* mutex, sem_t* sem_Full, sem_t* sem_Empty)
{
    queue_create_string_data(mutex, sem_Full, sem_Empty, RA);
}

void queue_create_log_data(pthread_mutex_t* mutex, sem_t* sem_Full, sem_t* sem_Empty)
{
    queue_create_string_data(mutex, sem_Full, sem_Empty, LOGGER);
}


void queue_destroy_RA_data(void)
{
    queue_destroy_string_data(RA);
}

void queue_destroy_log_data(void)
{
    queue_destroy_string_data(LOGGER);
}


void queue_enqueue_RA(char* data, unsigned short size)
{
    queue_enqueue_string(data, size, RA);
}

void queue_enqueue_log(char* data, unsigned short size)
{
    queue_enqueue_string(data, size, LOGGER);
}


queue_string_data_record* queue_dequeue_RA(void)
{
    return queue_dequeue_string(RA);
}

queue_string_data_record* queue_dequeue_log(void)
{   
    return queue_dequeue_string(LOGGER);
}

queue_string_data* queue_get_RA_data_instance()
{
    return queue_get_string_data_instance(RA);
}

queue_string_data* queue_get_log_data_instance()
{
    return queue_get_string_data_instance(LOGGER);
}

size_t queue_is_RA_data_null()
{
    return queue_is_string_data_null(RA);
}

size_t queue_is_log_data_null()
{
    return queue_is_string_data_null(LOGGER);
}

void queue_create_AP_data(pthread_mutex_t* mutex, sem_t* AP_Full, sem_t* AP_Empty)
{
    queue_number_data* newData = (queue_number_data*)calloc(1,sizeof(queue_number_data));
    if(newData == NULL)
    {
        fprintf(stderr, "Error allocating memory for queue_AP_data struct, exiting\n");
        return;
    }
    queue_number_data_record** arr = (queue_number_data_record**)calloc(default_queue_size, sizeof(queue_number_data_record*));
    if(arr == NULL)
    {
        fprintf(stderr, "Error allocating memory for circular queue array, exiting\n");
        return;
    }
    for(size_t i = 0; i < default_queue_size; i++)
    {
        queue_number_data_record* new_record = (queue_number_data_record*)calloc(1, sizeof(queue_number_data_record));
        if(arr == NULL)
        {
            fprintf(stderr, "Error allocating memory for circular queue record, exiting\n");
            return;
        }
        arr[i] = new_record;
    }

    newData->status = STATUS_NULL;
    newData->mutex = mutex;
    newData->number_data_sem_Empty = AP_Empty;
    newData->number_data_sem_Full = AP_Full;
    newData->number_data_arr = arr;

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
        if(AP_data->number_data_arr[i] != NULL)
        {
            if(AP_data->number_data_arr[i]->number_data != NULL)
            {
                free(AP_data->number_data_arr[i]->number_data);
                AP_data->number_data_arr[i]->number_data = NULL;
            }
            free(AP_data->number_data_arr[i]);
            AP_data->number_data_arr[i] = NULL;
        }
    }
    if(AP_data->number_data_arr != NULL)
    {
        free(AP_data->number_data_arr);
        AP_data->number_data_arr = NULL;
    }

    free(AP_data);
    AP_data = NULL;
}

void queue_enqueue_AP(double* data)
{
    unsigned short new_index = AP_data->in++ & (default_queue_size-1);
    AP_data->number_data_arr[new_index]->number_data = data;
}

queue_number_data_record* queue_dequeue_AP(void)
{
    unsigned short out_index = AP_data->out++ & (default_queue_size-1);
    return AP_data->number_data_arr[out_index];
}

queue_number_data* queue_get_AP_data_instance(void)
{
    return AP_data;
}

size_t queue_is_AP_data_null(void)
{
    if(AP_data == NULL)
    {
        return failure;
    }
    return success;
}


void queue_create_all(pthread_mutex_t mutexes[NUMBER_OF_SHARED_DATA_OBJECTS], sem_t sem_full[NUMBER_OF_SHARED_DATA_OBJECTS],sem_t sem_empty[NUMBER_OF_SHARED_DATA_OBJECTS])
{
    for(size_t i = RA_ID; i < NUMBER_OF_SHARED_DATA_OBJECTS; i++)
    {
        queues_constructors[i](&mutexes[i],&sem_full[i],&sem_empty[i]);
    }
}

void queue_destroy_all(void)
{
    for(size_t i = RA_ID; i < NUMBER_OF_SHARED_DATA_OBJECTS; i++)
    {
        queues_destructors[i]();
    }
}

size_t queue_check_null_all(void)
{
    if(queue_is_RA_data_null())
    {
        printf("Error creating RA_data, exiting from main\n");
        return failure;
    }
    if(queue_is_AP_data_null())
    {
        printf("Error creating RA_data, exiting from main\n");
        return failure;
    }
    if(queue_is_log_data_null())
    {
        printf("Error creating log_data, exiting from main\n");
        return failure;
    }
    return success;
}

