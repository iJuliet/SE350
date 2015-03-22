#ifndef SYSCALLS_H_
#define SYSCALLS_H_


#include "rtx.h"

#define KCD_PROC_ID 12
#define CRT_PROC_ID 13

void crt_process(void);
void kcd_process(void);
void set_up_sys_procs(PROC_INIT*);

#endif
