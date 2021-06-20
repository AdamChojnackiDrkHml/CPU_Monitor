#ifndef READER_H
#define READER_H

#include<stdlib.h>


typedef struct reader_data 
{
    pthread_mutex_t* mutex;
    char* data;
    unsigned short status;
    char pad[6];
} reader_data;

static FILE *fp;

static const size_t data_chunk_size = 256; 
static const size_t status_succes = 0;
static const size_t status_failure = 1;

static const size_t max_fopen_tries = 5;

static size_t control = 1;
static const size_t control_end = 0;

extern void*  reader_task(void *arg);

extern reader_data* reader_createReaderData(pthread_mutex_t* mutex, char* buffer);
extern void reader_end_task(void);


#endif
