
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include "../headers/reader.h"
#include "../headers/analyzer.h"
#include "../headers/printer.h"
#include "../headers/queue.h"
#include "../headers/global.h"
static pthread_t Reader, Analyzer, Printer;
static pthread_mutex_t RA_Mutex, AP_Mutex;
static sem_t RA_Empty, RA_Full, AP_Empty, AP_Full;

static unsigned short RA_Mutex_Flag, RA_Empty_Flag, RA_Full_Flag;
static unsigned short AP_Mutex_Flag, AP_Empty_Flag, AP_Full_Flag;
static volatile sig_atomic_t done  = 0;


static void destroy_semaphore(sem_t* sem, unsigned short sem_flag)
{
    if(sem_flag)
    {
        sem_destroy(sem);
    }
}

static void destroy_mutex(pthread_mutex_t* mutex, unsigned short mtx_flag)
{
    if(mtx_flag)
    {
        pthread_mutex_destroy(mutex);
    }
}

static noreturn void end_protocol(unsigned short exit_status)
{
    pthread_mutex_lock(&RA_Mutex);
    pthread_mutex_unlock(&RA_Mutex);
    queue_destroy_RA_data();
    queue_destroy_AP_data();
    destroy_mutex(&RA_Mutex, RA_Mutex_Flag);
    destroy_mutex(&AP_Mutex, AP_Mutex_Flag);
    destroy_semaphore(&RA_Full, RA_Full_Flag);
    destroy_semaphore(&RA_Empty,RA_Empty_Flag);
    destroy_semaphore(&AP_Full, AP_Full_Flag);
    destroy_semaphore(&AP_Empty,AP_Empty_Flag);
    exit(exit_status);
}

static void terminate_handler(int n)
{
    printf("should be dead\n");
    done = 1;
    write(0, "\nProgram End\n", 13);
    write(0, &n, 1);
}

static noreturn void exit_error(char error_message[static 1])
{
    printf("%s", error_message);
    end_protocol(EXIT_FAILURE);
}

int main(void)
{
    signal(SIGTERM, *terminate_handler);
    printf("%d\n", getpid());
   
    if(pthread_mutex_init(&RA_Mutex, NULL) != 0)
    {
        exit_error("Error creating mutex, exiting from main \n");
    }
    RA_Mutex_Flag = 1;

    if(sem_init(&RA_Empty,0,queue_size) != 0)
    {
        exit_error("Error creating empty semaphore, exiting from main \n");
    }
    RA_Empty_Flag = 1;

    if(sem_init(&RA_Full,0,0) != 0)
    {
        exit_error("Error creating full semaphore, exiting from main \n");
    }
    RA_Full_Flag = 1;

    if(pthread_mutex_init(&AP_Mutex, NULL) != 0)
    {
        exit_error("Error creating mutex, exiting from main \n");
    }
    AP_Mutex_Flag = 1;

    if(sem_init(&AP_Empty,0,queue_size) != 0)
    {
        exit_error("Error creating empty semaphore, exiting from main \n");
    }
    AP_Empty_Flag = 1;

    if(sem_init(&AP_Full,0,0) != 0)
    {
        exit_error("Error creating full semaphore, exiting from main \n");
    }
    AP_Full_Flag = 1;
    
    
    queue_create_RA_data(&RA_Mutex, &RA_Full, &RA_Empty);
    queue_create_AP_data(&AP_Mutex, &AP_Full, &AP_Empty);

    if(queue_is_RA_data_null())
    {
        exit_error("Error creating RA_data, exiting from main\n");
    }

    if(queue_is_AP_data_null())
    {
        exit_error("Error creating RA_data, exiting from main\n");
    }

  
    pthread_create(&Reader, NULL, &reader_task, NULL);
    pthread_create(&Analyzer, NULL, &analyzer_task, NULL);
    pthread_create(&Reader, NULL, &printer_task, NULL);
    while(!done);
    
    reader_call_exit(); 
    printf("reader dead\n");
    pthread_join(Reader, NULL); 

    analyzer_call_exit();
    printf("analyzer dead\n");
    pthread_join(Analyzer, NULL);

    printer_call_exit();
    printf("printer dead\n");
    pthread_join(Printer, NULL);
    
    printf("THIS IS HOW I DO IT\n");
    end_protocol(EXIT_SUCCESS);

}
