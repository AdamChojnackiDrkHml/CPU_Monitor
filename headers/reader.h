#ifndef READER_H
#define READER_H

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

typedef struct reader_data 
{
    pthread_mutex_t* mutex;
    char** data;
} reader_data;

static FILE *fp;

extern void*  reader_task(void *arg);
extern int reader_closeFile(void);
extern reader_data* reader_createReaderData(pthread_mutex_t* mutex, char** buffer);

#endif
