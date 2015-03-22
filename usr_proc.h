/**
 * @file:   usr_proc.h
 * @brief:  Four user processes for tests and one proc for printing test result header file
 * @author: G013
 * @date:   2014/02/05
 */
 
#ifndef USR_PROC_H_
#define USR_PROC_H

#define RTX_ERR -1
#define RTX_OK  0

#define DEFAULT 0
#define KCD_REG 1
#define CRT_REQ 2
#define COUNT_REPORT 4
#define WAKE_UP_TEN 5

#define KCD_PROC_ID 12
#define CRT_PROC_ID 13
#define PROC_A_ID 7
#define PROC_B_ID 8
#define PROC_C_ID 9

#define MAX_MSG_SIZE 64

#include "rtx.h"

typedef struct _msg_queue_node {
	int mtype; /* user defined message type */
	char mtext[MAX_MSG_SIZE]; /* body of the message */
	struct _msg_queue_node* next;
} MSG_QUEUE_NODE;

void set_test_procs(void);
void proc1(void);
void proc2(void);
void proc3(void);
void proc4(void);
void proc5(void);
void proc6(void);
void proc_a(void);
void proc_b(void);
void proc_c(void);
void printEndTestString(void);
MSG_QUEUE_NODE* dequeue(MSG_QUEUE_NODE*, MSG_QUEUE_NODE*);
void enqueue(MSG_QUEUE_NODE*, MSG_QUEUE_NODE* , MSG_QUEUE_NODE* );
#endif /* USR_PROC_H_ */
