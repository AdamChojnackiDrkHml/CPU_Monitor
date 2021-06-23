#ifndef GLOBAL_H
#define GLOBAL_H
#include <signal.h>
#define NUM_OF_THREADS 4

static const unsigned short default_queue_size = 16;
static const unsigned short logger_queue_size = 64;
static const size_t data_chunk_size = 256; 
static const unsigned short max_log_size = 256;
static enum threads{Reader_ID, Analyzer_ID, Printer_ID, Logger_ID} IDs;


#endif
