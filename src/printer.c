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
static volatile size_t analyzer_end_state = THREAD_WORKING;
static volatile size_t printer_control = 1;
static const size_t printer_first_scan = 1;
static double* printer_local_data = NULL;
static unsigned short analyzer_num_of_cpus = 0;

//Shared data and synchronization structures
static queue_number_data* analyzer_AP_data = NULL;
static pthread_mutex_t* printer_AP_mutex = NULL;
static sem_t* printer_AP_sem_full = NULL;
static sem_t* printer_AP_sem_empty = NULL;
static queue_number_data_record* printer_curr_record = NULL;

static void printer_printf_data(void);
static void printer_init_data(void);
static size_t printer_get_data(size_t is_first_scan);


void* printer_task(void *arg)
{
    printer_init_data();

    logger_log("PRINTER : Initialized shared data\n");
    watchdog_set_me_alive(Printer_ID);

    sem_wait(printer_AP_sem_full);
    pthread_mutex_lock(printer_AP_mutex);
        
    if(analyzer_AP_data->status == STATUS_END_BEFORE_WRITE || analyzer_AP_data->status == STATUS_SAFE_END)
    {
        pthread_mutex_unlock(printer_AP_mutex);
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
    analyzer_end_state = THREAD_END;
    return NULL;
}

static void printer_printf_data(void)
{
    fflush(stdout);
    
    for(size_t i = 0; i < analyzer_num_of_cpus; i++)
    {
        printf("CPU %zu\t %0.2lf%%\n", i, printer_local_data[i]);
    }
    printf("\n");
    return;
}

static void printer_init_data(void)
{
    analyzer_AP_data = queue_get_AP_data_instance();
    printer_AP_mutex = analyzer_AP_data->mutex;
    printer_AP_sem_full = analyzer_AP_data->number_data_sem_Full;
    printer_AP_sem_empty = analyzer_AP_data->number_data_sem_Empty;
}



static size_t printer_get_data(size_t is_first_scan)
{
    watchdog_set_me_alive(Printer_ID);
    if(is_first_scan)
    {
        analyzer_num_of_cpus = analyzer_AP_data->num_of_CPUs;
        printer_local_data = (double*)calloc(analyzer_num_of_cpus ,sizeof(double));
        if(printer_local_data == NULL)
        {
            pthread_mutex_unlock(printer_AP_mutex);
            return FAILURE;
        }
    }
    if(!is_first_scan)
    {
        sem_wait(printer_AP_sem_full);
        pthread_mutex_lock(printer_AP_mutex);
    }
    if(!is_first_scan && analyzer_AP_data->status == STATUS_SAFE_END)
    {
        pthread_mutex_unlock(printer_AP_mutex);
        return FAILURE;
    }
    printer_curr_record = queue_dequeue_AP();
    memcpy(printer_local_data,printer_curr_record->number_data,analyzer_num_of_cpus * sizeof(double));
    free(printer_curr_record->number_data);
    printer_curr_record->number_data = NULL;
    pthread_mutex_unlock(printer_AP_mutex);
    sem_post(printer_AP_sem_empty);
    watchdog_set_me_alive(Printer_ID);
    return SUCCESS;
}

void printer_call_exit(void)
{
    printer_control = END_THREAD;
    logger_log("MAIN in PRINTER :  Recieved signal to end\n");
    while(analyzer_end_state == THREAD_WORKING);
}
