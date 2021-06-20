#ifndef READER_H
#define READER_H



#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include <sys/stat.h>


typedef struct reader_data 
{
    pthread_mutex_t* mutex;
    char* data;
} reader_data;

static FILE *fp;
static struct stat st;

static const int data_chunk_size = 256; 
static size_t control = 1;

extern void*  reader_task(void *arg);
extern int reader_closeFile(void);
extern reader_data* reader_createReaderData(pthread_mutex_t* mutex, char* buffer);
extern void reader_endTask(void);


#endif
