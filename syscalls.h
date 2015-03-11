#ifndef SYSCALLS_H_
#define SYSCALLS_H_


#include "rtx.h"


void crt_process(void);
void kcd_process(void);
void set_up_sys_procs(PROC_INIT*);

#endif