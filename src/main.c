#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
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

//Pthread structs array
static pthread_t threads[NUMBER_OF_THREADS];

//Pointers to threads start and end functions
static void* (*thread_functions[NUMBER_OF_THREADS])(void *) = {&logger_task, &reader_task, &analyzer_task, &printer_task, &watchdog_task};
static void (*exit_functions[NUMBER_OF_THREADS])(void) = {logger_call_exit, reader_call_exit,analyzer_call_exit,printer_call_exit,watchdog_call_exit};

//Thread synchronization structures arrays
static pthread_mutex_t mutexes[NUMBER_OF_SHARED_DATA_OBJECTS];
static sem_t sem_full[NUMBER_OF_SHARED_DATA_OBJECTS];
static sem_t sem_empty[NUMBER_OF_SHARED_DATA_OBJECTS];

//Thread synchronization structures flags arrays
static unsigned short mutex_flags[NUMBER_OF_SHARED_DATA_OBJECTS]= {FAILURE, FAILURE, FAILURE};
static unsigned short sem_empty_flags[NUMBER_OF_SHARED_DATA_OBJECTS] = {FAILURE, FAILURE, FAILURE};
static unsigned short sem_full_flags[NUMBER_OF_SHARED_DATA_OBJECTS] = {FAILURE, FAILURE, FAILURE};

//Variable used to terminate program after SIGTERM is send
static volatile sig_atomic_t done  = 0;

//Exit handling functions
static noreturn void exit_error(char error_message[static 1]);
static noreturn void end_protocol(unsigned short exit_status);

//Shared data and synchronization structures initializiers
static void create_mutexes(void);
static void create_sem_full(void);
static void create_sem_empty(void);
static void create_synchronizers(void);
static void create_shared_data(void);

//Shared data and synchronization structures destructors
static void destroy_semaphore(sem_t* sem, unsigned short sem_flag);
static void destroy_mutex(pthread_mutex_t* mutex, unsigned short mtx_flag);
static void destroy_mutexes(void);
static void destroy_sem_full(void);
static void destroy_sem_empty(void);
static void destroy_synchronizers(void);
static void destroy_shared_data(void);

//SIGTERM handler
static void terminate_handler(int n);

int main(void)
{
    signal(SIGTERM, *terminate_handler);
    printf("%d\n", getpid());
   
    create_shared_data();

    for(size_t i = Logger_ID; i < NUMBER_OF_THREADS; i++)
    {
        pthread_create(&threads[i],NULL, thread_functions[i], NULL);
    }

    while(!done && !watchdog_raport());
    //sleep(3);

    exit_functions[Watchdog_ID]();
    pthread_join(threads[Watchdog_ID],NULL);
    printf("%s thread dead\n", thread_names[Watchdog_ID]);

    for(size_t i = Reader_ID; i < NUMBER_OF_THREADS - 1; i++)
    {
        exit_functions[i]();
        pthread_join(threads[i], NULL);
        printf("%s thread dead\n", thread_names[i]);
    }

    exit_functions[Logger_ID]();
    pthread_join(threads[Logger_ID],NULL);
    printf("%s thread dead\n", thread_names[Logger_ID]);

    printf("All end succes\n");
    end_protocol(EXIT_SUCCESS);

}

static void create_mutexes(void)
{
    for(size_t i = RA_ID; i < NUMBER_OF_SHARED_DATA_OBJECTS; i++)
    {
        if(pthread_mutex_init(&mutexes[i], NULL) != SUCCESS)
        {
            exit_error("Error creating mutex, exiting from main \n");
        }
        mutex_flags[i] = SUCCESS;
        
    }
}

static void create_sem_full(void)
{
    for(size_t i = RA_ID; i < NUMBER_OF_SHARED_DATA_OBJECTS; i++)
    {   
        if(sem_init(&sem_full[i],0,SEM_FULL_SIZE) != SUCCESS)
        {
            exit_error("Error creating full semaphore, exiting from main \n");
        }
        sem_full_flags[i] = SUCCESS;
    }
}

static void create_sem_empty(void)
{
    for(size_t i = RA_ID; i < NUMBER_OF_SHARED_DATA_OBJECTS; i++)
    {

        if(i != LOG_ID)
        {
            if(sem_init(&sem_empty[i],0,DEFAULT_QUEUE_SIZE) != SUCCESS)
            {
                exit_error("Error creating full semaphore, exiting from main \n");
            }
       
        }
        else
        {
            if(sem_init(&sem_empty[i],0,LOGGER_QUEUE_SIZE) != SUCCESS)
            {
                exit_error("Error creating full semaphore, exiting from main \n");
            }
        }
        sem_empty_flags[i] = SUCCESS;
    }
}

void create_synchronizers(void)
{
    create_mutexes();
    create_sem_empty();
    create_sem_full();
}

void create_shared_data(void)
{
    create_synchronizers();
    queue_create_all(mutexes, sem_full, sem_empty);
    if(queue_check_null_all())
    {
        exit_error("Error creating shared data, exiting");
    }
}
void destroy_semaphore(sem_t* sem, unsigned short sem_flag)
{
    if(!sem_flag)
    {
        sem_destroy(sem);
    }
}

void destroy_mutex(pthread_mutex_t* mutex, unsigned short mtx_flag)
{
    if(!mtx_flag)
    {
        pthread_mutex_destroy(mutex);
    }
}

void destroy_mutexes(void)
{
    for(size_t i = RA_ID; i < NUMBER_OF_SHARED_DATA_OBJECTS; i++)
    {
        destroy_mutex(&mutexes[i], mutex_flags[i]);
    }
}

void destroy_sem_full(void)
{
    for(size_t i = RA_ID; i < NUMBER_OF_SHARED_DATA_OBJECTS; i++)
    {
        destroy_semaphore(&sem_full[i], sem_full_flags[i]);
    }
}

void destroy_sem_empty(void)
{
    for(size_t i = RA_ID; i < NUMBER_OF_SHARED_DATA_OBJECTS; i++)
    {
        destroy_semaphore(&sem_empty[i], sem_empty_flags[i]);
    }
}

void destroy_synchronizers(void)
{
    destroy_mutexes();
    destroy_sem_empty();
    destroy_sem_full();
}

void destroy_shared_data(void)
{
    destroy_synchronizers();
    queue_destroy_all();
}

noreturn void end_protocol(unsigned short exit_status)
{
    destroy_shared_data();
    exit(exit_status);
}

void terminate_handler(int n)
{
    write(0,"recieved signal",16);
    done = 1;
}

noreturn void exit_error(char error_message[static 1])
{
    printf("%s", error_message);
    end_protocol(EXIT_FAILURE);
}
