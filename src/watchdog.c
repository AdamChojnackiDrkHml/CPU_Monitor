#include "../headers/watchdog.h"
#include <pthread.h>
#include <unistd.h>
#include "../headers/global.h"


static volatile size_t watchdog_control = 1;
static const size_t watchdog_control_end = 0;
    
static volatile size_t end_succes = 0;
static unsigned char state = 1;
static volatile _Atomic(char) watchdog_check[NUM_OF_THREADS];
 
void* watchdog_task(void* arg)
{

    while(watchdog_control)
    {
        sleep(2);
        for(size_t i = 0; i < NUM_OF_THREADS; i++)
        {
            if(!watchdog_check[i])
            {
                state = 0;
                break;
            }
            watchdog_check[i] = 0;
        }
    }
    while(watchdog_control);
    end_succes = 1;
}

void watchdog_call_exit(void)
{
    watchdog_control = watchdog_control_end;
    while(!end_succes);
}

void watchdog_set_me_alive(unsigned char ID)
{
    watchdog_check[ID] = 1;
}

unsigned char watchdog_raport(void)
{
    return state;
}