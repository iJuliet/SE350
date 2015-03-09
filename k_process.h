/**
 * @file:   k_process.h
 * @brief:  process management hearder file
 * @author: Yiqing Huang
 * @author: Thomas Reidemeister
 * @date:   2014/01/17
 * NOTE: Assuming there are only two user processes in the system
 */

#ifndef K_PROCESS_H_
#define K_PROCESS_H_

#include "k_rtx.h"

/* ----- Definitions ----- */

#define INITIAL_xPSR 0x01000000        /* user process initial xPSR value */

/* ----- Functions ----- */

void process_init(void);               /* initialize all procs in the system */
PCB *scheduler(void);                  /* pick the pid of the next to run process */
int process_switch(PCB *p_pcb_old);
int k_release_processor(void);           /* kernel release_process function */
void null_process(void);
void timer_i_process();
int set_process_priority(int process_id, int priority);
int get_process_priority(int process_id);
void rpq_enqueue(PCB* pcb);
PCB* rpq_dequeue(void);
void bq_enqueue(PCB* pcb);
PCB* bq_dequeue(void);
PCB* get_current_proc(void);

extern U32 *alloc_stack(U32 size_b);   /* allocate stack for a process */
extern void __rte(void);               /* pop exception stack frame */
extern void set_test_procs(void);      /* test process initial set up */

#endif /* ! K_PROCESS_H_ */
