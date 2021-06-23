#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../headers/queue.h"
#include "../headers/global.h"

static queue_RA_data* RA_data = NULL;
static queue_AP_data* AP_data = NULL;

void queue_create_RA_data(pthread_mutex_t* mutex, sem_t* RA_Full, sem_t* RA_Empty)
{
    queue_RA_data* newData = (queue_RA_data*)calloc(1, sizeof(queue_RA_data));
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
    newData->RA_Empty = RA_Empty;
    newData->RA_Full = RA_Full;
    newData->data = arr;

    RA_data = newData;
}

void queue_destroy_RA_data(void)
{
    if(RA_data == NULL)
    {
        return;
    }
    for(size_t i = 0; i < queue_size; i++)
    {
        if(RA_data->data[i] != NULL)
        {
            if(RA_data->data[i]->data != NULL)
            {
                free(RA_data->data[i]->data);
                RA_data->data[i]->data = NULL;
            }
            free(RA_data->data[i]);
            RA_data->data[i] = NULL;
        }
    }
    if(RA_data->data != NULL)
    {
        free(RA_data->data);
        RA_data->data = NULL;
    }

    free(RA_data);
    RA_data = NULL;
}

void queue_enqueue_RA(char* data, unsigned short size)
{
    unsigned short new_index = RA_data->in++ & (queue_size-1);
    RA_data->data[new_index]->data = data;
    RA_data->data[new_index]->size = size;
    
}

queue_RA_record* queue_dequeue_RA(void)
{
    return RA_data->data[RA_data->out++ & (queue_size-1)];
}

queue_RA_data* queue_get_RA_data_instance(void)
{
    return RA_data;
}

size_t queue_is_RA_data_null(void)
{
    if(RA_data == NULL)
    {
        return 1;
    }
    return 0;
}

void queue_create_AP_data(pthread_mutex_t* mutex, sem_t* AP_Full, sem_t* AP_Empty)
{
    queue_AP_data* newData = (queue_AP_data*)malloc(sizeof(queue_AP_data));
    if(newData == NULL)
    {
        fprintf(stderr, "Error allocating memory for queue_AP_data struct, exiting\n");
        return;
    }
    queue_AP_record** arr = (queue_AP_record**)calloc(queue_size, sizeof(queue_AP_record*));
    if(arr == NULL)
    {
        fprintf(stderr, "Error allocating memory for circular queue array, exiting\n");
        return;
    }
    for(size_t i = 0; i < queue_size; i++)
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
    for(size_t i = 0; i < queue_size; i++)
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
    unsigned short new_index = AP_data->in++ & (queue_size-1);
    AP_data->data[new_index]->data = data;
}

queue_AP_record* queue_dequeue_AP(void)
{
    unsigned short out_index = AP_data->out++ & (queue_size-1);
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

