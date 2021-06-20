#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "../headers/reader.h"

static pthread_t Reader;
static pthread_mutex_t R_A_Mutex;
static char* buffer;
static reader_data* R_data;



static void handler(int n)
{
    reader_endTask();  
    printf("should be dead\n");

    write(0, "\nProgram End\n", 13);
    write(0, &n, 1);
}

int main(void)
{
    signal(SIGTERM, *handler);
    
   
    if(pthread_mutex_init(&R_A_Mutex, NULL) != 0)
    {
        printf("Error creating mutex, exiting \n");
        exit(EXIT_FAILURE);
    }

    buffer = (char*)malloc(sizeof(char)* 10000);
    if(buffer == NULL)
    {
        printf("Error creating buffer, exiting \n");
        exit(EXIT_FAILURE);
    }
    
    R_data = reader_createReaderData(&R_A_Mutex, buffer);

    pthread_create(&Reader, NULL, &reader_task, &R_data);
    pthread_join(Reader, NULL);
    printf("THIS IS HOW I DO IT\n");

    free(R_data);
    free(buffer);     
    pthread_mutex_destroy(&R_A_Mutex);
    exit(EXIT_SUCCESS);
}
