#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <limits.h>
#include <sys/stat.h>
#include "../headers/reader.h"


static size_t reader_open_file(void)
{
    for(size_t i = 1; i < 6; i++)
    {
        fp = fopen("/proc/stat", "r");
        if (fp != NULL)
        {
            return 1;
        }
        printf("Error openning /proc/stat %zu time'n", i);
    }
    printf("5th time failure openning file, exiting \n");
    return 0;
}


static int reader_close_file(void)
{

    fclose(fp);

    return 0;
}

static char* reader_rFile_to_buffer(void)
{
    size_t readDataSizeMultiplier = 1;
    char* readData = (char*)malloc(sizeof(char));

    if(readData == NULL)
    {
        printf("Error, failed allocating memory for readData, exiting \n");
        return NULL;
    }

    char* buffer = (char*)malloc(sizeof(char)*data_chunk_size);
    if(buffer == NULL)
    {
        printf("Error, failed allocating memory for 256 bytes buffer, exiting \n");
        free(buffer);
        return NULL;
    }

    strcpy(readData, "");

    while(fgets(buffer,data_chunk_size, fp) != NULL )
    {
        char* temp = (char*)realloc(readData, sizeof(char) * ((data_chunk_size * readDataSizeMultiplier) + 1));
        if(temp == NULL)
        {
            printf("Error, failed allocating memory for 256 bytes buffer, exiting \n");
            free(buffer);
            free(readData);
            return NULL;
        }
        else if(readData != temp)
        {
            readData = temp;
        }
        temp = NULL;
        free(temp);
        readDataSizeMultiplier++;
        strcat(readData, buffer);
    }

    free(buffer);

    return readData;
}


void*  reader_task(void *arg)
{   
    
    reader_data** r_data = (reader_data**)(arg);
    pthread_mutex_t* mutex = (*r_data)->mutex;
    while(control)
    {
        
        pthread_mutex_lock(mutex);

        if(reader_open_file() == 0)
        {
            (*r_data)->status = 1;
            return NULL;
        }

        char* scannedData = reader_rFile_to_buffer();
        if(scannedData == NULL)
        {
            printf("Error, retriveing data from function failed, exiting\n");
            (*r_data)->status = 1;
            return NULL;

        }

        (*r_data)->data = scannedData;
      

        printf("%s", (*r_data)->data);
        pthread_mutex_unlock((*r_data)->mutex);

        reader_close_file();
        free(scannedData);
        sleep(2);
    }
    return NULL;
}



void reader_end_task(void)
{
    control = 0;
}


reader_data* reader_createReaderData(pthread_mutex_t* mutex, char* buffer)
{
    reader_data* newData = (reader_data*)malloc(sizeof(reader_data));
    if(newData == NULL)
    {
        printf("Error allocating memory for reader_data struct, exiting\n");
        return NULL;
    }
    newData->status = 0;
    newData->mutex = mutex;
    newData->data = buffer;

    return newData;
}
