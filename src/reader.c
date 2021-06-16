#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "../headers/reader.h"

void*  reader_task(void *arg)
{   

    reader_data* r_data = (reader_data*)(arg);
    while(1)
    {
        pthread_mutex_lock(r_data->mutex);
        char * line = NULL;
        size_t len = 0;

        fp = fopen("/proc/stat", "r");
        if (fp == NULL)
        {
            exit(EXIT_FAILURE); 
        }
        
        while (getline(&line, &len, fp) != -1) 
        {
            printf("%s", line);
        }

        reader_closeFile();
        pthread_mutex_unlock(r_data->mutex);
        free(line);
        sleep(2);
    }
}

int reader_closeFile(void)
{
    return fclose(fp);
}

reader_data* reader_createReaderData(pthread_mutex_t* mutex, char** buffer)
{
    reader_data* newData = (reader_data*)malloc(sizeof(reader_data));
    if(newData == NULL)
    {
        printf("Error allocating memory for reader_data struct, exiting\n");
        exit(EXIT_FAILURE);
    }
    newData->mutex = mutex;
    newData->data = buffer;

    return newData;
}
