#ifndef QUEUE_H
#define QUEUE_H

#include "pthread.h"

typedef struct queue_RA_data 
{
    pthread_mutex_t* mutex;
    char* data;
    unsigned short status;
    char pad[6];
} queue_RA_data;

extern queue_RA_data* queue_create_RA_data(pthread_mutex_t* mutex);
extern void queue_destroy_RA_data(queue_RA_data** RA_data);

#endif
