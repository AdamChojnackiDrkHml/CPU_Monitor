#include "../headers/watchdog.h"
#include <pthread.h>
#include <unistd.h>
#include "../headers/global.h"

static const size_t watchdog_check_time = 2;
static const size_t watchdog_num_of_threads_to_watch = 4;
static volatile size_t watchdog_control = 1;
static const size_t watchdog_control_end = 0;

static const char thread_state_unknown = 1;
static const char thread_state_alive = 0;     

static const char watchdog_all_threads_alive = 1;
static const char watchdog_thread_is_dead = 1;



static volatile size_t end_state = THREAD_WORKING;
static unsigned char state = watchdog_all_threads_alive;
static volatile _Atomic(char) watchdog_check[watchdog_num_of_threads_to_watch];
 
void* watchdog_task(void* arg)
{

    while(watchdog_control)
    {
        sleep(watchdog_check_time);
        for(size_t i = 0; i < watchdog_num_of_threads_to_watch; i++)
        {
            if(watchdog_check[i] == thread_state_unknown)
            {
                state = watchdog_thread_is_dead;
                break;
            }
            watchdog_check[i] = thread_state_unknown;
        }
    }
    while(watchdog_control);
    end_state = THREAD_END;
    return NULL;
}

void watchdog_call_exit(void)
{
    watchdog_control = END_THREAD;
    while(end_state == THREAD_WORKING);
}

void watchdog_set_me_alive(unsigned char ID)
{
    watchdog_check[ID] = thread_state_alive;
}

unsigned char watchdog_raport(void)
{
    return state;
}
