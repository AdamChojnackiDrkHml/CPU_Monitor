#ifndef LOGGER_H
#define LOGGER_H

extern void* logger_task(void * arg);
extern void logger_call_exit(void);
extern void logger_log(char* log);
#endif
