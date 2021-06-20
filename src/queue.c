#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../headers/queue.h"



queue_RA_data* queue_create_RA_data(pthread_mutex_t* mutex, char* buffer)
{
    queue_RA_data* newData = (queue_RA_data*)malloc(sizeof(queue_RA_data));
    if(newData == NULL)
    {
        fprintf(stderr, "Error allocating memory for queue_RA_data struct, exiting\n");
        return NULL;
    }
    newData->status = 0;
    newData->mutex = mutex;
    newData->data = buffer;

    return newData;
}
