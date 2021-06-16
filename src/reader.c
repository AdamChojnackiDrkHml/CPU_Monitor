#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../headers/reader.h"

void*  reader_task(void *arg)
{   
    char parameter = *(char*)arg;

    while(1)
    {
        char * line = NULL;
        size_t len = 0;
        size_t read;

        fp = fopen("/proc/stat", "r");
        if (fp == NULL)
        {
            exit(EXIT_FAILURE);
        }
        
        while (getline(&line, &len, fp) != -1) 
        {
            printf("%s", line);
        }

        reader_closeFile();
        free(line);
        sleep(2);
    }
}

size_t reader_closeFile()
{
    return fclose(fp);
}
