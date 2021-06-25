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
static FILE *reader_fp;
static const size_t reader_max_fopen_tries = 5;
static volatile size_t reader_control = 1;
static unsigned short reader_read_data_size_multiplier = 1;
static volatile size_t reader_end_state = THREAD_WORKING;
static unsigned short reader_is_read = FAILURE;
static unsigned short reader_is_error = FAILURE;
static char* reader_scanned_data = NULL;
static size_t reader_file_open_flag = FAILURE;

//Shared data and synchronization structures
static queue_string_data* reader_RA_data;
static pthread_mutex_t* reader_RA_mutex;
static sem_t* reader_RA_sem_Full;
static sem_t* reader_RA_sem_Empty;

static void reader_init_data(void);
static size_t reader_open_file(void);
static size_t reader_close_file(void);
static size_t reader_reset_data(void);
static char* reader_read_file_to_buffer(void);
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
            reader_RA_data->status = STATUS_ERROR_END;
            break;
        }
        logger_log("READER : opened file\n");

        reader_scanned_data = reader_read_file_to_buffer();
        watchdog_set_me_alive(Reader_ID);

        if(reader_scanned_data == NULL)
        {
            logger_log("READER : Error, retriveing data from function failed, exiting\n");
            reader_is_error = SUCCESS;
            reader_RA_data->status = STATUS_ERROR_END;
            break;
        }
        reader_send_to_analyzer();
        
        watchdog_set_me_alive(Reader_ID);
        logger_log("READER : put data in queue\n");
        
        if(reader_reset_data())
        {
            reader_RA_data->status = STATUS_ERROR_END;
            break;
        }
        logger_log("READER : closed file\n");


        if(reader_is_read == FAILURE)
        {
            reader_is_read = SUCCESS;
        }
        
        sleep(1);
    }

    if(reader_is_read == FAILURE)
    {
        reader_RA_data->status = STATUS_END_BEFORE_WRITE;
    }
    else if(reader_is_error == FAILURE)
    {
        reader_RA_data->status = STATUS_SAFE_END;
    }
    sem_post(reader_RA_sem_Full);
    logger_log("READER : Freed all recources, exiting\n");
    reader_reset_data();
    while(reader_control);

    reader_end_state = THREAD_END;
    return NULL;
}


static void reader_init_data(void)
{
    reader_RA_data = queue_get_RA_data_instance();
    reader_RA_mutex = reader_RA_data->mutex;
    reader_RA_sem_Full = reader_RA_data->string_data_sem_Full;
    reader_RA_sem_Empty = reader_RA_data->string_data_sem_Empty;
}

static size_t reader_open_file(void)
{

    for(size_t i = 1; i <= reader_max_fopen_tries; i++)
    {
        reader_fp = fopen("/proc/stat", "r");
        if (reader_fp != NULL)
        {   
            reader_file_open_flag = SUCCESS;
            return SUCCESS;
        }
        logger_log("READER : Error openning /proc/stat\n");
    }
    logger_log("READER : 5th time failure openning file, exiting \n");
    return FAILURE;
}

static size_t reader_close_file(void)
{
    if(reader_file_open_flag)
    {
        return SUCCESS;
    }
    if(fclose(reader_fp) == EOF)
    {
        return FAILURE;
    }
    reader_file_open_flag = FAILURE;
    reader_fp = NULL;
    return SUCCESS;
}

static size_t reader_reset_data(void)
{
    reader_scanned_data = NULL;
    reader_read_data_size_multiplier = 1;
    return reader_close_file();
}

static char* reader_read_file_to_buffer(void)
{
    
    char* read_data = (char*)malloc(sizeof(char));

    if(read_data == NULL)
    {
        logger_log("READER : Error, failed allocating memory for read_data, exiting \n");
        return NULL;
    }

    char* buffer = (char*)malloc(sizeof(char)*DATA_CHUNK_SIZE);
    if(buffer == NULL)
    {
        logger_log("READER : Error, failed allocating memory for 256 bytes buffer, exiting \n");
        free(read_data);
        return NULL;
    }

    strcpy(read_data, "");

    while(fgets(buffer,DATA_CHUNK_SIZE, reader_fp) != NULL )
    {
        char* temp = (char*)malloc(sizeof(char) * ((DATA_CHUNK_SIZE * reader_read_data_size_multiplier) + 1));
        if(temp == NULL)
        {
            logger_log("READER : Error, failed reallocating memory for extended data buffer, exiting \n");
            free(buffer);
            free(read_data);
            return NULL;
        }
        strcpy(temp, read_data);
        free(read_data);
        read_data = temp;
        reader_read_data_size_multiplier++;
        strcat(read_data, buffer);
    }

    free(buffer);
    buffer = NULL;
    return read_data;
}

static void reader_send_to_analyzer(void)
{
    sem_wait(reader_RA_sem_Empty);
    pthread_mutex_lock(reader_RA_mutex);
    queue_enqueue_RA(reader_scanned_data, reader_read_data_size_multiplier);
    reader_RA_data->status = STATUS_WORKING;
    pthread_mutex_unlock(reader_RA_mutex);
    sem_post(reader_RA_sem_Full);
}

void reader_call_exit(void)
{
    reader_control = END_THREAD;
    logger_log("MAIN in READER :  Recieved signal to end\n");
    while(reader_end_state == THREAD_WORKING);
}



