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

typedef struct queue_AP_record
{
    double* data;
} queue_AP_record;

typedef struct queue_AP_data 
{
    pthread_mutex_t* mutex;
    sem_t* AP_Empty;
    sem_t* AP_Full;
    queue_AP_record** data;
    unsigned short status;
    unsigned short in;
    unsigned short out;
    unsigned short num_of_CPUs;
} queue_AP_data;


extern void queue_create_RA_data(pthread_mutex_t* mutex, sem_t* RA_Full, sem_t* RA_Empty);
extern void queue_destroy_RA_data(void);
extern void queue_enqueue_RA(char* data, unsigned short size);
extern queue_RA_record* queue_dequeue_RA(void);
extern queue_RA_data* queue_get_RA_data_instance(void);
extern size_t queue_is_RA_data_null(void);

extern void queue_create_AP_data(pthread_mutex_t* mutex, sem_t* AP_Full, sem_t* AP_Empty);
extern void queue_destroy_AP_data(void);
extern void queue_enqueue_AP(double* data);
extern queue_AP_record* queue_dequeue_AP(void);
extern queue_AP_data* queue_get_AP_data_instance(void);
extern size_t queue_is_AP_data_null(void);


#endif
