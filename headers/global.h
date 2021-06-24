#ifndef GLOBAL_H
#define GLOBAL_H
#include <signal.h>
#define NUMBER_OF_THREADS 5
#define NUMBER_OF_SHARED_DATA_OBJECTS 3
#define LOGGER_QUEUE_SIZE 64
#define DEFAULT_QUEUE_SIZE 16
#define DATA_CHUNK_SIZE 256
#define MAX_LOG_SIZE 256
#define SEM_FULL_SIZE 0
#define SUCCESS 0
#define FAILURE 1
#define END_THREAD 0
#define THREAD_WORKING 1
#define THREAD_END 0

static enum threads{Logger_ID, Reader_ID, Analyzer_ID, Printer_ID, Watchdog_ID} IDs;
static enum shared_data_name{RA_ID, AP_ID, LOG_ID} shared_data_name;
static char* thread_names[NUMBER_OF_THREADS] = {"Logger", "Reader", "Analyzer", "Printer", "Watchdog"};



#endif
