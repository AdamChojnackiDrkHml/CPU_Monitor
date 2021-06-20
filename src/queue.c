#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../headers/queue.h"



queue_RA_data* queue_create_RA_data(pthread_mutex_t* mutex)
{
    queue_RA_data* newData = (queue_RA_data*)malloc(sizeof(queue_RA_data));
    if(newData == NULL)
    {
        fprintf(stderr, "Error allocating memory for queue_RA_data struct, exiting\n");
        return NULL;
    }
    newData->status = 0;
    newData->mutex = mutex;
    newData->data = NULL;

    return newData;
}

void queue_destroy_RA_data(queue_RA_data** RA_data)
{
    if(*RA_data != NULL)
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
