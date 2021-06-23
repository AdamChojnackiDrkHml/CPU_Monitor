#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "../headers/printer.h"
#include "../headers/queue.h"
#include "../headers/global.h"

static unsigned short cpus_counter = 0;
static queue_AP_record* curr_record = NULL;
static double* printer_local_data = NULL;

static volatile size_t printer_control = 1;
static const size_t printer_control_end = 0;
static volatile size_t end_succes = 0;

static void printer_printf_data(void)
{
    fflush(stdout);
    for(size_t i = 0; i < cpus_counter; i++)
    {
        printf("CPU %zu\t %0.2lf%%\n", i, printer_local_data[i]);
    }
    write(0,"a", 1);
    return;
}
void* printer_task(void *arg)
{
    queue_AP_data* AP_data = queue_get_AP_data_instance();
    pthread_mutex_t* mutex = AP_data->mutex;
    sem_t* AP_Full = AP_data->AP_Full;
    sem_t* AP_Empty = AP_data->AP_Empty;
    write(0,"u",1);

    sem_wait(AP_Full);
    pthread_mutex_lock(mutex);
        
    if(AP_data->status == 3 || AP_data->status == 0)
    {
        write(0,"a",1);
        pthread_mutex_unlock(mutex);
        pthread_exit(NULL);
    }
    write(0,"d",1);
    cpus_counter = AP_data->num_of_CPUs;
    curr_record = queue_dequeue_AP();
    printer_local_data = (double*)calloc(cpus_counter ,sizeof(double));
    memcpy(printer_local_data,curr_record->data,cpus_counter * sizeof(double));
    free(curr_record->data);
    curr_record->data = NULL;

    pthread_mutex_unlock(mutex);
    sem_post(AP_Empty);
    printer_printf_data();

    while(printer_control)
    {
        sem_wait(AP_Full);
        pthread_mutex_lock(mutex);
        if(AP_data->status == 0)
        {
            pthread_mutex_unlock(mutex);
            break;
        }
        curr_record = queue_dequeue_AP();
        memcpy(printer_local_data,curr_record->data,cpus_counter * sizeof(double));
        free(curr_record->data);
        curr_record->data = NULL;
        pthread_mutex_unlock(mutex);
        sem_post(AP_Empty);
        printer_printf_data();

    }
    free(printer_local_data);
    end_succes = 1;
    return NULL;
}

void printer_call_exit(void)
{
    printer_control = printer_control_end;
    while(!end_succes);
}
