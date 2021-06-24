#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "../headers/printer.h"
#include "../headers/queue.h"
#include "../headers/logger.h"
#include "../headers/global.h"
#include "../headers/watchdog.h"

//Printer working variables
static volatile size_t end_state = THREAD_WORKING;
static volatile size_t printer_control = 1;
static size_t printer_first_scan = 1;
static double* printer_local_data = NULL;
static unsigned short cpus_counter = 0;

//Shared data and synchronization structures
static queue_number_data* AP_data = NULL;
static pthread_mutex_t* mutex = NULL;
static sem_t* AP_Full = NULL;
static sem_t* AP_Empty = NULL;
static queue_number_data_record* curr_record = NULL;

static void printer_printf_data(void);
static void printer_init_data(void);
static size_t printer_get_data(size_t is_first_scan);


void* printer_task(void *arg)
{
    printer_init_data();

    logger_log("PRINTER : Initialized shared data\n");
    watchdog_set_me_alive(Printer_ID);

    sem_wait(AP_Full);
    pthread_mutex_lock(mutex);
        
    if(AP_data->status == STATUS_END_BEFORE_WRITE || AP_data->status == STATUS_SAFE_END)
    {
        pthread_mutex_unlock(mutex);
        printer_control = FAILURE;
    }
    if(printer_control)
    {
        printer_get_data(printer_first_scan);
       
        logger_log("PRINTER : Scanned first data\n");

        printer_printf_data();
        logger_log("PRINTER : Printed first data\n");

        while(printer_control)
        {
            watchdog_set_me_alive(Printer_ID);
            if(printer_get_data(!printer_first_scan))
            {
                break;
            }
            logger_log("PRINTER : Recieved data\n");

            printer_printf_data();
            logger_log("PRINTER : Printed data\n");

        }
        free(printer_local_data);
        logger_log("PRINTER : Freed all recources, exiting\n");
    }
    while(printer_control);
    end_state = THREAD_END;
    return NULL;
}

static void printer_init_data(void)
{
    AP_data = queue_get_AP_data_instance();
    mutex = AP_data->mutex;
    AP_Full = AP_data->number_data_sem_Full;
    AP_Empty = AP_data->number_data_sem_Empty;
}

static void printer_printf_data(void)
{
    fflush(stdout);
    
    for(size_t i = 0; i < cpus_counter; i++)
    {
        printf("CPU %zu\t %0.2lf%%\n", i, printer_local_data[i]);
    }
    printf("\n");
    return;
}

static size_t printer_get_data(size_t is_first_scan)
{
    watchdog_set_me_alive(Printer_ID);
    if(is_first_scan)
    {
        cpus_counter = AP_data->num_of_CPUs;
        printer_local_data = (double*)calloc(cpus_counter ,sizeof(double));
    }
    if(!is_first_scan)
    {
        sem_wait(AP_Full);
        pthread_mutex_lock(mutex);
    }
    if(!is_first_scan && AP_data->status == STATUS_SAFE_END)
    {
        pthread_mutex_unlock(mutex);
        return FAILURE;
    }
    curr_record = queue_dequeue_AP();
    memcpy(printer_local_data,curr_record->number_data,cpus_counter * sizeof(double));
    free(curr_record->number_data);
    curr_record->number_data = NULL;
    pthread_mutex_unlock(mutex);
    sem_post(AP_Empty);
    watchdog_set_me_alive(Printer_ID);
    return SUCCESS;
}

void printer_call_exit(void)
{
    printer_control = END_THREAD;
    while(end_state == THREAD_WORKING);
}
