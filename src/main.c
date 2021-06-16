#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "../headers/reader.h"
static pthread_t Reader;






static void handler(int n)
{
    reader_closeFile();
    write(0, "\nProgram End\n", 13);
    write(0, &n, 1);
    pthread_cancel(Reader);
}

int main(void)
{
    signal(SIGTERM, *handler);
    int successStart = 0;
    pthread_create(&Reader, NULL, &reader_task, &successStart);
    pthread_join(Reader, NULL);
    
    return 0;
}
