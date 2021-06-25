#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../headers/queue.h"
#include "../headers/global.h"
#define LOGGER 1
#define RA 0

static queue_string_data* queue_RA_data = NULL;
static queue_string_data* queue_log_data = NULL;
static queue_number_data* queue_AP_data = NULL;

static void queue_create_string_data(pthread_mutex_t* mutex, sem_t* sem_Full, sem_t* sem_Empty, size_t is_logger);
static void queue_destroy_string_data(size_t is_logger);
static void queue_enqueue_string(char* data, unsigned short size, size_t is_logger);
static queue_string_data_record* queue_dequeue_string(size_t is_logger);
static queue_string_data* queue_get_string_data_instance(size_t is_logger);
static size_t queue_is_string_data_null(size_t is_logger);
static void queue_create_RA_data(pthread_mutex_t* mutex, sem_t* sem_Full, sem_t* sem_Empty);
static void queue_create_log_data(pthread_mutex_t* mutex, sem_t* sem_Full, sem_t* sem_Empty);
static void queue_destroy_RA_data(void);
static void queue_destroy_log_data(void);
static size_t queue_is_RA_data_null(void);
static size_t queue_is_log_data_null(void);
static void queue_create_AP_data(pthread_mutex_t* mutex, sem_t* AP_Full, sem_t* AP_Empty);
static void queue_destroy_AP_data(void);
static size_t queue_is_AP_data_null(void);

static void (*queues_constructors[NUMBER_OF_SHARED_DATA_OBJECTS])(pthread_mutex_t* mutex, sem_t* sem_full, sem_t* sem_empty) = { queue_create_RA_data,queue_create_AP_data,queue_create_log_data};
static void (*queues_destructors[NUMBER_OF_SHARED_DATA_OBJECTS])(void) = { queue_destroy_RA_data,queue_destroy_AP_data,queue_destroy_log_data};

static void queue_create_string_data(pthread_mutex_t* mutex, sem_t* sem_Full, sem_t* sem_Empty, size_t is_logger)
{
    queue_string_data* new_data = (queue_string_data*)calloc(1, sizeof(queue_string_data));
    unsigned short queue_size = is_logger ? LOGGER_QUEUE_SIZE : DEFAULT_QUEUE_SIZE;
    if(new_data == NULL)
    {
        fprintf(stderr, "Error allocating memory for queue_RA_data struct, exiting\n");
        return;
    }
    queue_string_data_record** new_data_arr = (queue_string_data_record**)calloc(queue_size, sizeof(queue_string_data_record*));
    if(new_data_arr == NULL)
    {
        fprintf(stderr, "Error allocating memory for circular queue array, exiting\n");
        return;
    }

    for(size_t i = 0; i < queue_size; i++)
    {
        queue_string_data_record* new_record = (queue_string_data_record*)calloc(1, sizeof(queue_string_data_record));
        if(new_data_arr == NULL)
        {
            fprintf(stderr, "Error allocating memory for circular queue record, exiting\n");
            return;
        }
        new_data_arr[i] = new_record;
    }

    new_data->status = STATUS_NULL;
    new_data->mutex = mutex;
    new_data->string_data_sem_Empty = sem_Empty;
    new_data->string_data_sem_Full = sem_Full;
    new_data->string_data_arr = new_data_arr;

    if(is_logger)
    {
       queue_log_data = new_data;
    }
    else
    {
        queue_RA_data = new_data;
    }
}

static void queue_destroy_string_data(size_t is_logger)
{
    queue_string_data* to_destroy = is_logger ? queue_log_data : queue_RA_data;
    unsigned short queue_size = is_logger ? LOGGER_QUEUE_SIZE : DEFAULT_QUEUE_SIZE;
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

    if(is_logger)
    {
        queue_log_data = NULL;
    }
    else
    {
        queue_RA_data = NULL;
    }
}

static void queue_enqueue_string(char* data, unsigned short size, size_t is_logger)
{
    
    if(is_logger)
    {
        unsigned short new_index = queue_log_data->in;
        queue_log_data->string_data_arr[new_index]->string_data = data;
        queue_log_data->string_data_arr[new_index]->size = size;
        queue_log_data->in = (queue_log_data->in + 1) % LOGGER_QUEUE_SIZE;
    }
    else
    {
        unsigned short new_index = queue_RA_data->in;
        queue_RA_data->string_data_arr[new_index]->string_data = data;
        queue_RA_data->string_data_arr[new_index]->size = size;
        queue_RA_data->in = (queue_RA_data->in + 1) % DEFAULT_QUEUE_SIZE;
    }

}

static queue_string_data_record* queue_dequeue_string(size_t is_logger)
{
    queue_string_data_record* to_return;
    if (is_logger)
    {
        to_return = queue_log_data->string_data_arr[queue_log_data->out];
        queue_log_data->out = (queue_log_data->out + 1) % LOGGER_QUEUE_SIZE;
    }
    else
    {
        to_return = queue_RA_data->string_data_arr[queue_RA_data->out];
        queue_RA_data->out = (queue_RA_data->out + 1) % DEFAULT_QUEUE_SIZE;
    }
    return to_return;
}

static queue_string_data* queue_get_string_data_instance(size_t is_logger)
{
    return is_logger ? queue_log_data : queue_RA_data;
}

static size_t queue_is_string_data_null(size_t is_logger)
{
    queue_string_data* to_check = is_logger ? queue_log_data : queue_RA_data;
    if(to_check == NULL)
    {
        return FAILURE;
    }
    return SUCCESS;
}

static void queue_create_RA_data(pthread_mutex_t* mutex, sem_t* sem_full, sem_t* sem_Empty)
{
    queue_create_string_data(mutex, sem_full, sem_Empty, RA);
}

static void queue_create_log_data(pthread_mutex_t* mutex, sem_t* sem_Full, sem_t* sem_Empty)
{
    queue_create_string_data(mutex, sem_Full, sem_Empty, LOGGER);
}


static void queue_destroy_RA_data(void)
{
    queue_destroy_string_data(RA);
}

static void queue_destroy_log_data(void)
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

static size_t queue_is_RA_data_null()
{
    return queue_is_string_data_null(RA);
}

static size_t queue_is_log_data_null()
{
    return queue_is_string_data_null(LOGGER);
}

static void queue_create_AP_data(pthread_mutex_t* mutex, sem_t* sem_full, sem_t* sem_empty)
{
    queue_number_data* new_data = (queue_number_data*)calloc(1,sizeof(queue_number_data));
    if(new_data == NULL)
    {
        fprintf(stderr, "Error allocating memory for queue_AP_data struct, exiting\n");
        return;
    }
    queue_number_data_record** new_data_arr = (queue_number_data_record**)calloc(DEFAULT_QUEUE_SIZE, sizeof(queue_number_data_record*));
    if(new_data_arr == NULL)
    {
        fprintf(stderr, "Error allocating memory for circular queue array, exiting\n");
        return;
    }
    for(size_t i = 0; i < DEFAULT_QUEUE_SIZE; i++)
    {
        queue_number_data_record* new_record = (queue_number_data_record*)calloc(1, sizeof(queue_number_data_record));
        if(new_data_arr == NULL)
        {
            fprintf(stderr, "Error allocating memory for circular queue record, exiting\n");
            return;
        }
        new_data_arr[i] = new_record;
    }

    new_data->status = STATUS_NULL;
    new_data->mutex = mutex;
    new_data->number_data_sem_Empty = sem_empty;
    new_data->number_data_sem_Full = sem_full;
    new_data->number_data_arr = new_data_arr;

    queue_AP_data = new_data;
    return;
}

static void queue_destroy_AP_data(void)
{
    if(queue_AP_data == NULL)
    {
        return;
    }
    for(size_t i = 0; i < DEFAULT_QUEUE_SIZE; i++)
    {
        if(queue_AP_data->number_data_arr[i] != NULL)
        {
            if(queue_AP_data->number_data_arr[i]->number_data != NULL)
            {
                free(queue_AP_data->number_data_arr[i]->number_data);
                queue_AP_data->number_data_arr[i]->number_data = NULL;
            }
            free(queue_AP_data->number_data_arr[i]);
            queue_AP_data->number_data_arr[i] = NULL;
        }
    }
    if(queue_AP_data->number_data_arr != NULL)
    {
        free(queue_AP_data->number_data_arr);
        queue_AP_data->number_data_arr = NULL;
    }

    free(queue_AP_data);
    queue_AP_data = NULL;
}

void queue_enqueue_AP(double* data)
{
    unsigned short new_index = queue_AP_data->in;
    queue_AP_data->number_data_arr[new_index]->number_data = data;
    queue_AP_data->in = (queue_AP_data->in + 1) % DEFAULT_QUEUE_SIZE;
}

queue_number_data_record* queue_dequeue_AP(void)
{
    queue_number_data_record* to_return = queue_AP_data->number_data_arr[queue_AP_data->out];
    queue_AP_data->out = (queue_AP_data->out + 1) % DEFAULT_QUEUE_SIZE;
    return to_return;
}

queue_number_data* queue_get_AP_data_instance(void)
{
    return queue_AP_data;
}

static size_t queue_is_AP_data_null(void)
{
    if(queue_AP_data == NULL)
    {
        return FAILURE;
    }
    return SUCCESS;
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
        return FAILURE;
    }
    if(queue_is_AP_data_null())
    {
        printf("Error creating RA_data, exiting from main\n");
        return FAILURE;
    }
    if(queue_is_log_data_null())
    {
        printf("Error creating log_data, exiting from main\n");
        return FAILURE;
    }
    return SUCCESS;
}

