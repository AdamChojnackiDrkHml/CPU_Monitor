#ifndef QUEUE_H
#define QUEUE_H

#include "pthread.h"
#include <semaphore.h>

typedef struct queue_RA_data 
{
    pthread_mutex_t* mutex;
    sem_t* RA_Empty;
    sem_t* RA_Full;
    char* data;
    unsigned short status;
    unsigned short size;
    char pad[4];
} queue_RA_data;

typedef struct queue_AP_data 
{
    pthread_mutex_t* mutex;
    sem_t* AP_Empty;
    sem_t* AP_Full;
    char* data;
} queue_AP_data;

extern queue_RA_data* queue_create_RA_data(pthread_mutex_t* mutex, sem_t* RA_Full, sem_t* RA_Empty);
extern void queue_destroy_RA_data(queue_RA_data** RA_data);

extern queue_AP_data* queue_create_AP_data(pthread_mutex_t* mutex, sem_t* AP_Full, sem_t* AP_Empty);
extern void queue_destroy_AP_data(queue_RA_data** AP_data);

#endif
