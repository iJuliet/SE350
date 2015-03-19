#ifndef WALL_CLOCK_H_
#define WALL_CLOCK_H_


#define DEFAULT 0
#define KCD_REG 1
#define CRT_REQ 2
#define NOTIFY_WALL_CLOCK 3

#define WALL_CLOCK_PROC_ID 11
#define KCD_PROC_ID 12
#define CRT_PROC_ID 13

#include "rtx.h"
void wc_process(void);

#endif