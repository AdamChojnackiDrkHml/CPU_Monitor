#ifndef ANALYZER_H
#define ANALYZER_H

#include "queue.h"

static size_t control = 1;
static const size_t control_end = 0;

static const size_t status_succes = 0;
static const size_t status_failure = 1;

extern void*  analyzer_task(void *arg);
extern void analyzer_end_task(void);
#endif
