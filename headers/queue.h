#ifndef QUEUE_H
#define QUEUE_H

#include "pthread.h"
#include <semaphore.h>

static enum status_type{STATUS_SAFE_END, STATUS_ERROR_END, STATUS_WORKING, STATUS_END_BEFORE_WRITE,STATUS_NULL} status_type;

typedef struct queue_string_data_record
{
    char* string_data;
    unsigned short size;
    char pad[6];
} queue_string_data_record;

typedef struct queue_string_data 
{
    pthread_mutex_t* mutex;
    sem_t* string_data_sem_Empty;
    sem_t* string_data_sem_Full;
    queue_string_data_record** string_data_arr;
    unsigned short status;
    unsigned short in;
    unsigned short out;
    char pad[2];
} queue_string_data;

typedef struct queue_number_data_record
{
    double* number_data;
} queue_number_data_record;

typedef struct queue_number_data 
{
    pthread_mutex_t* mutex;
    sem_t* number_data_sem_Empty;
    sem_t* number_data_sem_Full;
    queue_number_data_record** number_data_arr;
    unsigned short status;
    unsigned short in;
    unsigned short out;
    unsigned short num_of_CPUs;
} queue_number_data;

extern void queue_create_all(pthread_mutex_t mutexes[3], sem_t sem_full[3],sem_t sem_empty[3]);
extern void queue_destroy_all(void);
extern size_t queue_check_null_all(void);


extern void queue_enqueue_RA(char* data, unsigned short size);
extern queue_string_data_record* queue_dequeue_RA(void);
extern queue_string_data* queue_get_RA_data_instance(void);

extern void queue_enqueue_AP(double* data);
extern queue_number_data_record* queue_dequeue_AP(void);
extern queue_number_data* queue_get_AP_data_instance(void);

extern void queue_enqueue_log(char* data, unsigned short size);
extern queue_string_data_record* queue_dequeue_log(void);
extern queue_string_data* queue_get_log_data_instance(void);



#endif
