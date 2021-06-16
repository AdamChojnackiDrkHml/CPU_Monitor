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






static void handler(int n)
{
    reader_closeFile();
    write(0, "\nProgram End\n", 13);
    write(0, &n, 1);
    pthread_cancel(Reader);
}

int main(void)
{
    signal(SIGTERM, *handler);
    int successStart = 0;
    pthread_mutex_t R_A_Mutex;
   
    if(pthread_mutex_init(&R_A_Mutex, NULL) != 0)
    {
        printf("Error creating mutex, exiting \n");
        exit(EXIT_FAILURE);
    }
     char* buffer = (char*)malloc(sizeof(char)* 2000);
    if(buffer == NULL)
    {
        printf("Error creating buffer, exiting \n");
        exit(EXIT_FAILURE);
    }
    
    reader_data* R_data = reader_createReaderData(&R_A_Mutex, &buffer);

    pthread_create(&Reader, NULL, &reader_task, &R_data);
    pthread_join(Reader, NULL);
    
    return 0;
}
