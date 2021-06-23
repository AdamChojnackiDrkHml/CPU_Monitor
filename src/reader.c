#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../headers/reader.h"
#include "../headers/queue.h"
#include "../headers/global.h"


static FILE *fp;


static const size_t reader_status_succes = 0;
static const size_t reader_status_failure = 1;
static volatile size_t end_succes = 0;
static const size_t max_fopen_tries = 5;

static size_t reader_control = 1;
static const size_t reader_control_end = 0;
static unsigned short last_read_Data_Size_Multiplier = 1;
static unsigned short is_read = 0;

static size_t reader_open_file(void)
{

    for(size_t i = 1; i <= max_fopen_tries; i++)
    {
        fp = fopen("/proc/stat", "r");
        if (fp != NULL)
        {
            return reader_status_succes;
        }
        fprintf(stderr, "Error openning /proc/stat %zu time\n", i);
    }
    fprintf(stderr, "5th time failure openning file, exiting \n");
    return reader_status_failure;
}


static int reader_close_file(void)
{
    if(fclose(fp) == EOF)
    {
        return reader_status_failure;
    }
    fp = NULL;
    return reader_status_succes;
}

static char* reader_rFile_to_buffer(void)
{
    
    char* readData = (char*)malloc(sizeof(char));

    if(readData == NULL)
    {
        fprintf(stderr, "Error, failed allocating memory for readData, exiting \n");
        return NULL;
    }

    char* buffer = (char*)malloc(sizeof(char)*data_chunk_size);
    if(buffer == NULL)
    {
        fprintf(stderr, "Error, failed allocating memory for 256 bytes buffer, exiting \n");
        free(buffer);
        return NULL;
    }

    strcpy(readData, "");

    while(fgets(buffer,data_chunk_size, fp) != NULL )
    {
        char* temp = (char*)malloc(sizeof(char) * ((data_chunk_size * last_read_Data_Size_Multiplier) + 1));
        if(temp == NULL)
        {
            fprintf(stderr, "Error, failed reallocating memory for extended data buffer, exiting \n");
            free(buffer);
            free(readData);
            return NULL;
        }
        strcpy(temp, readData);
        free(readData);
        readData = temp;
        last_read_Data_Size_Multiplier++;
        strcat(readData, buffer);
    }

    free(buffer);
    buffer = NULL;
    end_succes = 1;
    return readData;
}


void*  reader_task(void *arg)
{   
    
    queue_RA_data* RA_data = queue_get_RA_data_instance();
    pthread_mutex_t* mutex = RA_data->mutex;
    sem_t* RA_Full = RA_data->RA_Full;
    sem_t* RA_Empty = RA_data->RA_Empty;
    while(reader_control)
    {
        
        if(reader_open_file())
        {
            RA_data->status = 1;
            return NULL;
        }

        char* scannedData = reader_rFile_to_buffer();

        sem_wait(RA_Empty);
        pthread_mutex_lock(mutex);
        if(scannedData == NULL)
        {
            fprintf(stderr, "Error, retriveing data from function failed, exiting\n");
            RA_data->status = reader_status_failure;
            return NULL;

        }
        queue_enqueue_RA(scannedData, last_read_Data_Size_Multiplier);
        RA_data->status = 2;
        pthread_mutex_unlock(RA_data->mutex);
        sem_post(RA_Full);
        if(!is_read)
        {
            is_read = 1;
        }
        last_read_Data_Size_Multiplier = 1;
        reader_close_file();
        scannedData = NULL;
        sleep(1);
    }
    if(!is_read)
    {
        RA_data->status = 3;
        
    }
    else 
    {
        RA_data->status = 0;
    }
    sem_post(RA_Full);
    return NULL;
}


void reader_call_exit(void)
{
    reader_control = reader_control_end;
    while(!end_succes);
}



