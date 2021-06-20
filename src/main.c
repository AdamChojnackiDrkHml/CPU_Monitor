#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include "../headers/reader.h"

static pthread_t Reader;
static pthread_mutex_t R_A_Mutex;
static char* buffer;
static reader_data* R_data;


static void terminate_handler(int n)
{
    reader_end_task();  
    printf("should be dead\n");

    write(0, "\nProgram End\n", 13);
    write(0, &n, 1);
}


static noreturn void end_protocol(unsigned short exit_status)
{
    if(R_data != NULL)
    {
        free(R_data);
        R_data = NULL;
    }
    if(buffer != NULL)
    {
        free(buffer);
        buffer = NULL;     
    }
    pthread_mutex_destroy(&R_A_Mutex);
    exit(exit_status);
}

static noreturn void exit_error(char error_message[static 1])
{
    printf("%s", error_message);
    end_protocol(EXIT_FAILURE);
}

int main(void)
{
    signal(SIGTERM, *terminate_handler);
    
   
    if(pthread_mutex_init(&R_A_Mutex, NULL) != 0)
    {
        exit_error("Error creating mutex, exiting \n");
    }

    buffer = (char*)malloc(sizeof(char)* 10000);
    if(buffer == NULL)
    {
        exit_error("Error creating buffer, exiting \n");
    }
    
    R_data = reader_createReaderData(&R_A_Mutex, buffer);

    if(R_data == NULL)
    {
        exit_error("Error creating buffer, exiting \n");
    }

    pthread_create(&Reader, NULL, &reader_task, &R_data);
    pthread_join(Reader, NULL);
    if(R_data->status)
    {
        exit_error("ERROR in printer, exit failure from main");
    }

    printf("THIS IS HOW I DO IT\n");
    end_protocol(EXIT_SUCCESS);
}
