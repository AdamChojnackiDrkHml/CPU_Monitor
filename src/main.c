
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
#include "../headers/logger.h"
#include "../headers/watchdog.h"
#include "../headers/global.h"
static pthread_t Reader, Analyzer, Printer, Logger, Watchdog;
static pthread_mutex_t RA_Mutex, AP_Mutex, log_Mutex;
static sem_t RA_Empty, RA_Full, AP_Empty, AP_Full, log_Empty, log_Full;

static unsigned short RA_Mutex_Flag, RA_Empty_Flag, RA_Full_Flag;
static unsigned short AP_Mutex_Flag, AP_Empty_Flag, AP_Full_Flag;
static unsigned short log_Mutex_Flag, log_Empty_Flag, log_Full_Flag;

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
    queue_destroy_RA_data();
    queue_destroy_AP_data();
    queue_destroy_log_data();
    destroy_mutex(&RA_Mutex, RA_Mutex_Flag);
    destroy_mutex(&AP_Mutex, AP_Mutex_Flag);
    destroy_mutex(&log_Mutex, log_Mutex_Flag);
    destroy_semaphore(&RA_Full, RA_Full_Flag);
    destroy_semaphore(&RA_Empty,RA_Empty_Flag);
    destroy_semaphore(&AP_Full, AP_Full_Flag);
    destroy_semaphore(&AP_Empty,AP_Empty_Flag);
    destroy_semaphore(&log_Full, log_Full_Flag);
    destroy_semaphore(&log_Empty,log_Empty_Flag);
    exit(exit_status);
}

static void terminate_handler(int n)
{
    done = 1;
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

    if(sem_init(&RA_Empty,0,default_queue_size) != 0)
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

    if(sem_init(&AP_Empty,0,default_queue_size) != 0)
    {
        exit_error("Error creating empty semaphore, exiting from main \n");
    }
    AP_Empty_Flag = 1;

    if(sem_init(&AP_Full,0,0) != 0)
    {
        exit_error("Error creating full semaphore, exiting from main \n");
    }
    AP_Full_Flag = 1;

    if(pthread_mutex_init(&log_Mutex, NULL) != 0)
    {
        exit_error("Error creating mutex, exiting from main \n");
    }
    log_Mutex_Flag = 1;

    if(sem_init(&log_Empty,0,logger_queue_size) != 0)
    {
        exit_error("Error creating empty semaphore, exiting from main \n");
    }
    log_Empty_Flag = 1;

    if(sem_init(&log_Full,0,0) != 0)
    {
        exit_error("Error creating full semaphore, exiting from main \n");
    }
    log_Full_Flag = 1;
    
    
    queue_create_RA_data(&RA_Mutex, &RA_Full, &RA_Empty);
    queue_create_AP_data(&AP_Mutex, &AP_Full, &AP_Empty);
    queue_create_log_data(&log_Mutex, &log_Full, &log_Empty);

    if(queue_is_RA_data_null())
    {
        exit_error("Error creating RA_data, exiting from main\n");
    }

    if(queue_is_AP_data_null())
    {
        exit_error("Error creating RA_data, exiting from main\n");
    }

    if(queue_is_log_data_null())
    {
        exit_error("Error creating log_data, exiting from main\n");
    }

    pthread_create(&Logger, NULL, &logger_task, NULL);
    pthread_create(&Reader, NULL, &reader_task, NULL);
    pthread_create(&Analyzer, NULL, &analyzer_task, NULL);
    pthread_create(&Printer, NULL, &printer_task, NULL);
    pthread_create(&Watchdog, NULL, &watchdog_task, NULL);

    while(!done && watchdog_raport());

    watchdog_call_exit();
    pthread_join(Watchdog, NULL);
    printf("watchdog dead\n");

    reader_call_exit(); 
    pthread_join(Reader, NULL); 
    printf("reader dead\n");

    analyzer_call_exit();
    pthread_join(Analyzer, NULL);
    printf("analyzer dead\n");

    printer_call_exit();
    pthread_join(Printer, NULL);
    printf("printer dead\n");

    logger_call_exit();
    pthread_join(Logger, NULL);
    printf("logger dead\n");

    printf("THIS IS HOW I DO IT\n");
    end_protocol(EXIT_SUCCESS);

}
