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

queue_RA_data* queue_create_RA_data(pthread_mutex_t* mutex, char* buffer);

#endif
