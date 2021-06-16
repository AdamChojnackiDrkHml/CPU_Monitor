#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

static pthread_t Reader;
static FILE *fp;


static void*  _task_reader(void *arg);
static void*  _task_reader(void *arg)
{
    char parameter = *(char*)arg;

    

    while(1)
    {
        char * line = NULL;
        size_t len = 0;
        ssize_t read;

        fp = fopen("/proc/stat", "r");
        if (fp == NULL)
        {
            exit(EXIT_FAILURE);
        }
        
        while (getline(&line, &len, fp) != -1) 
        {
            printf("%s", line);
        }

        fclose(fp);
        free(line);
        sleep(2);
    }
}

static void handler(int n)
{
    n += 0x30;
    fclose(fp);
    write(0, "\nProgram End\n", 13);
    write(0, &n, 1);
    pthread_cancel(Reader);
}

int main(void)
{
    signal(SIGTERM, *handler);
    int successStart = 0;
    pthread_create(&Reader, NULL, &_task_reader, &successStart);
    pthread_join(Reader, NULL);
    
    return 0;
}
