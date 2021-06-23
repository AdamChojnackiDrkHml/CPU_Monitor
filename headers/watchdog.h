#ifndef WATCHDOG_H
#define WATCHDOG_H

extern void* watchdog_task(void* arg);
extern void watchdog_call_exit(void);
extern void watchdog_set_me_alive(unsigned char ID);
extern unsigned char watchdog_raport(void);
#endif
