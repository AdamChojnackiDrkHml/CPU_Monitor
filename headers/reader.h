#ifndef READER_H
#define READER_H

#include<stdio.h>
#include<stdlib.h>

static FILE *fp;

extern void*  reader_task(void *arg);
extern size_t reader_closeFile();

#endif
