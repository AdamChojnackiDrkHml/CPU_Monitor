#ifndef QUEUE_H
#define QUEUE_H

#include "pthread.h"
#include <semaphore.h>

typedef struct queue_RA_record
{
    char* data;
    unsigned short size;
    char pad[6];
} queue_RA_record;

typedef struct queue_RA_data 
{
    pthread_mutex_t* mutex;
    sem_t* RA_Empty;
    sem_t* RA_Full;
    queue_RA_record** data;
    unsigned short status;
    unsigned short in;
    unsigned short out;
    char pad[2];
} queue_RA_data;

typedef struct queue_AP_data 
{
    pthread_mutex_t* mutex;
    sem_t* AP_Empty;
    sem_t* AP_Full;
    char* data;
} queue_AP_data;


extern queue_RA_data* queue_create_RA_data(pthread_mutex_t* mutex, sem_t* RA_Full, sem_t* RA_Empty);
extern void queue_destroy_RA_data(void);
extern void queue_enqueue_RA(char* data, unsigned short size);
extern queue_RA_record* queue_dequeue_RA(void);

extern queue_AP_data* queue_create_AP_data(pthread_mutex_t* mutex, sem_t* AP_Full, sem_t* AP_Empty);
extern void queue_destroy_AP_data(queue_RA_data** AP_data);

#endif
