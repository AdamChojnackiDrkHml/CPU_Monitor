#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../headers/reader.h"
#include "../headers/queue.h"
#include "../headers/global.h"
#include "../headers/logger.h"
#include "../headers/watchdog.h"

//Reader working variables
static FILE *fp;
static volatile size_t end_state = THREAD_WORKING;
static const size_t max_fopen_tries = 5;
static volatile size_t reader_control = 1;
static unsigned short last_read_Data_Size_Multiplier = 1;
static unsigned short is_read = FAILURE;
static unsigned short is_error = FAILURE;
static char* scannedData = NULL;
static size_t reader_file_open_flag = FAILURE;

//Shared data and synchronization structures
static queue_string_data* RA_data;
static pthread_mutex_t* RA_mutex;
static sem_t* RA_Full;
static sem_t* RA_Empty;

static void reader_init_data(void);
static size_t reader_reset_data(void);
static size_t reader_open_file(void);
static size_t reader_close_file(void);
static char* reader_rFile_to_buffer(void);
static void reader_send_to_analyzer(void);

void* reader_task(void *arg)
{   
    reader_init_data();

    logger_log("READER : Initialized shared data\n");

    while(reader_control)
    {
        watchdog_set_me_alive(Reader_ID);
        if(reader_open_file())
        {
            RA_data->status = STATUS_ERROR_END;
            break;
        }
        logger_log("READER : opened file\n");

        scannedData = reader_rFile_to_buffer();
        watchdog_set_me_alive(Reader_ID);

        if(scannedData == NULL)
        {
            logger_log("READER : Error, retriveing data from function failed, exiting\n");
            is_error = SUCCESS;
            RA_data->status = STATUS_ERROR_END;
            break;
        }
        reader_send_to_analyzer();
        
        watchdog_set_me_alive(Reader_ID);
        logger_log("READER : put data in queue\n");
        
        if(reader_reset_data())
        {
            RA_data->status = STATUS_ERROR_END;
            break;
        }
        logger_log("READER : closed file\n");


        if(is_read == FAILURE)
        {
            is_read = SUCCESS;
        }
        
        sleep(1);
    }

    if(is_read == FAILURE)
    {
        RA_data->status = STATUS_END_BEFORE_WRITE;
    }
    else if(is_error == FAILURE)
    {
        RA_data->status = STATUS_SAFE_END;
    }
    sem_post(RA_Full);
    logger_log("READER : Freed all recources, exiting\n");
    reader_reset_data();
    while(reader_control);

    end_state = THREAD_END;
    return NULL;
}


void reader_init_data(void)
{
    RA_data = queue_get_RA_data_instance();
    RA_mutex = RA_data->mutex;
    RA_Full = RA_data->string_data_sem_Full;
    RA_Empty = RA_data->string_data_sem_Empty;
}

size_t reader_reset_data(void)
{
    scannedData = NULL;
    last_read_Data_Size_Multiplier = 1;
    return reader_close_file();
}

size_t reader_open_file(void)
{

    for(size_t i = 1; i <= max_fopen_tries; i++)
    {
        fp = fopen("/proc/stat", "r");
        if (fp != NULL)
        {   
            reader_file_open_flag = SUCCESS;
            return SUCCESS;
        }
        logger_log("READER : Error openning /proc/stat\n");
    }
    logger_log("READER : 5th time failure openning file, exiting \n");
    return FAILURE;
}

size_t reader_close_file(void)
{
    if(reader_file_open_flag)
    {
        return SUCCESS;
    }
    if(fclose(fp) == EOF)
    {
        return FAILURE;
    }
    reader_file_open_flag = FAILURE;
    fp = NULL;
    return SUCCESS;
}

char* reader_rFile_to_buffer(void)
{
    
    char* readData = (char*)malloc(sizeof(char));

    if(readData == NULL)
    {
        logger_log("READER : Error, failed allocating memory for readData, exiting \n");
        return NULL;
    }

    char* buffer = (char*)malloc(sizeof(char)*DATA_CHUNK_SIZE);
    if(buffer == NULL)
    {
        logger_log("READER : Error, failed allocating memory for 256 bytes buffer, exiting \n");
        free(readData);
        return NULL;
    }

    strcpy(readData, "");

    while(fgets(buffer,DATA_CHUNK_SIZE, fp) != NULL )
    {
        char* temp = (char*)malloc(sizeof(char) * ((DATA_CHUNK_SIZE * last_read_Data_Size_Multiplier) + 1));
        if(temp == NULL)
        {
            logger_log("READER : Error, failed reallocating memory for extended data buffer, exiting \n");
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
    return readData;
}

void reader_send_to_analyzer(void)
{
    sem_wait(RA_Empty);
    pthread_mutex_lock(RA_mutex);
    queue_enqueue_RA(scannedData, last_read_Data_Size_Multiplier);
    RA_data->status = STATUS_WORKING;
    pthread_mutex_unlock(RA_mutex);
    sem_post(RA_Full);
}

void reader_call_exit(void)
{
    reader_control = END_THREAD;
    logger_log("MAIN in READER :  Recieved signal to end\n");
    while(end_state == THREAD_WORKING);
}



