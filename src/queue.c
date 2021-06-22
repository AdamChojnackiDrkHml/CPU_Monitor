#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../headers/queue.h"



queue_RA_data* queue_create_RA_data(pthread_mutex_t* mutex, sem_t* RA_Full, sem_t* RA_Empty)
{
    queue_RA_data* newData = (queue_RA_data*)malloc(sizeof(queue_RA_data));
    if(newData == NULL)
    {
        fprintf(stderr, "Error allocating memory for queue_RA_data struct, exiting\n");
        return NULL;
    }
    newData->status = 0;
    newData->mutex = mutex;
    newData->size = 0;
    newData->RA_Empty = RA_Empty;
    newData->RA_Full = RA_Full;
    newData->data = NULL;

    return newData;
}

void queue_destroy_RA_data(queue_RA_data** RA_data)
{
    if(*RA_data == NULL)
    {
        return;
    }
    if((*RA_data)->data != NULL)
    {
        free((*RA_data)->data);
        (*RA_data)->data = NULL;
    }

    free(*RA_data);
    *RA_data = NULL;
}


queue_AP_data* queue_create_AP_data(pthread_mutex_t* mutex, sem_t* AP_Full, sem_t* AP_Empty)
{
    queue_AP_data* newData = (queue_AP_data*)malloc(sizeof(queue_AP_data));
    if(newData == NULL)
    {
        fprintf(stderr, "Error allocating memory for queue_AP_data struct, exiting\n");
        return NULL;
    }
    newData->mutex = mutex;
    newData->AP_Empty = AP_Empty;
    newData->AP_Full = AP_Full;
    newData->data = NULL;

    return newData;
}

void queue_destroy_AP_data(queue_RA_data** AP_data)
{
    if(*AP_data != NULL)
    {
        return;
    }
    if((*AP_data)->data != NULL)
    {
        free((*AP_data)->data);
        (*AP_data)->data = NULL;
    }

    free(*AP_data);
    *AP_data = NULL;
}
