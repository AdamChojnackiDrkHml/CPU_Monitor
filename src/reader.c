#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <limits.h>
#include <sys/stat.h>
#include "../headers/reader.h"

void*  reader_task(void *arg)
{   
    size_t fail_counter = 0;
    reader_data** r_data = (reader_data**)(arg);
    pthread_mutex_t* mutex = (*r_data)->mutex;
    while(control)
    {
        
        pthread_mutex_lock(mutex);
        char* readData = (char*)malloc(sizeof(char));
        strcpy(readData, "");
        char* buffer = (char*)malloc(sizeof(char)*data_chunk_size);
        char* temp = NULL;
        size_t readDataSizeMultiplier = 1;
        fp = fopen("/proc/stat", "r");
        if (fp == NULL)
        {
            printf("Error openning /proc/stat %zu time'n", ++fail_counter);
            
            if(fail_counter == 5)
            {
                printf("5th time failure openning file, exiting \n");
                exit(EXIT_FAILURE); 
            }
            continue;
        }

       
        while(fgets(buffer,data_chunk_size, fp) != NULL )
        {
            temp = (char*)realloc(readData, sizeof(char) * ((data_chunk_size * readDataSizeMultiplier) + 1));
            if(temp == NULL)
            {
                exit(EXIT_FAILURE);
            }
            else if(readData != temp)
            {
                readData = temp;
            }
            temp = NULL;
            readDataSizeMultiplier++;
            strcat(readData, buffer);
        }

        (*r_data)->data = readData;
      

        printf("%s", (*r_data)->data);
        pthread_mutex_unlock((*r_data)->mutex);
        free(buffer);
        free(readData);
        free(temp);
        reader_closeFile();

        sleep(2);
    }
    return NULL;
}

void reader_endTask(void)
{
    control = 0;
}

int reader_closeFile(void)
{

    fclose(fp);

    return 0;
}

reader_data* reader_createReaderData(pthread_mutex_t* mutex, char* buffer)
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
