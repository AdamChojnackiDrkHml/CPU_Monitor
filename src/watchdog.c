#include "../headers/watchdog.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "../headers/global.h"
#include "../headers/logger.h"
#define NUM_OF_THREADS_TO_WATCH 4

//Watchdog working variables and resources
static volatile size_t watchdog_control = 1;
static const size_t watchdog_check_time = 2;
static const char watchdog_thread_state_unknown = 1;
static const char watchdog_thread_state_alive = 0;     
static const char watchdog_all_threads_alive = 0;
static const char watchdog_thread_is_dead = 1;
static volatile size_t watchdog_end_state = THREAD_WORKING;
static unsigned char watchdog_state_raport = watchdog_all_threads_alive;
static volatile _Atomic(char) watchdog_check[NUM_OF_THREADS_TO_WATCH] = {0,0,0,0};
 
void* watchdog_task(void* arg)
{
    while(watchdog_control && !watchdog_state_raport)
    {
        logger_log("WATCHDOG :  GOING SLEEP\n");
        sleep(watchdog_check_time);
        for(size_t i = 0; i < NUM_OF_THREADS_TO_WATCH; i++)
        {
            if(watchdog_check[i] == watchdog_thread_state_unknown)
            {
                watchdog_state_raport = watchdog_thread_is_dead;
                printf("%zu thread is dead\n", i);
                logger_log("WATCHDOG : THREAD IS DEAD!\n");
                break;
            }
            watchdog_check[i] = watchdog_thread_state_unknown;
        }
    }
    while(watchdog_control);
    watchdog_end_state = THREAD_END;
    return NULL;
}

void watchdog_call_exit(void)
{
    watchdog_control = END_THREAD;
    logger_log("MAIN in WATCHDOG : Recieved signal to end\n");
    while(watchdog_end_state == THREAD_WORKING);
}

void watchdog_set_me_alive(unsigned char ID)
{
    watchdog_check[ID] = watchdog_thread_state_alive;
}

unsigned char watchdog_raport(void)
{
    return watchdog_state_raport;
}
