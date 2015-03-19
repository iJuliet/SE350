/** 
 * @file:   k_rtx.h
 * @brief:  kernel deinitiation and data structure header file
 * @auther: Yiqing Huang
 * @date:   2014/01/17
 */
 
#ifndef K_RTX_H_
#define K_RTX_H_

/*----- Definitations -----*/

#define RTX_ERR -1
#define RTX_OK  0

#define NULL 0
#define NUM_TEST_PROCS 6
#define DEFAULT 0
#define KCD_REG 1
#define CRT_REQ 2
#define TOTAL_PROCS 13

#define MAX_MSG_SIZE 64

#define WALL_CLOCK_PROCESS 6
//index numbers for kernel processes
#define NULL_PROCESS 7
#define KCD_PROCESS 8
#define CRT_PROCESS 9
#define TIMER_I_PROCESS 10
#define UART_I_PROCESS 11
#define SET_PRIORITY_PROC 12

//kernel level process pids
#define NULL_PROC_ID 0
#define KCD_PROC_ID 12
#define CRT_PROC_ID 13
#define TIMER_PROC_ID 14
#define UART_PROC_ID 15


#ifdef DEBUG_0
#define USR_SZ_STACK 0x200         /* user proc stack size 512B   */
#else
#define USR_SZ_STACK 0x100         /* user proc stack size 218B  */
#endif /* DEBUG_0 */

/*----- Types -----*/
typedef unsigned char U8;
typedef unsigned int U32;

/* process states, note we only assume three states in this example */
typedef enum {NEW = 0, RDY, RUN, BLK_ON_MEM, BLK_ON_MSG, WAITING_FOR_INTERRUPT} PROC_STATE_E;  

/*msgbug struct
*/
typedef struct _msgbuf {
	int mtype; /* user defined message type */
	char mtext[MAX_MSG_SIZE]; /* body of the message */
	int sender_pid;
	int receiver_pid;
	int send_time;
	struct _msgbuf* next;
} msgbuf;

/*
  PCB data structure definition.
  You may want to add your own member variables
  in order to finish P1 and the entire project 
*/
typedef struct pcb 
{ 
	//struct pcb *mp_next;  /* next pcb, not used in this example */  
	U32 *mp_sp;		/* stack pointer of the process */
	U32 m_pid;		/* process id */
	int m_priority; 
	PROC_STATE_E m_state;   /* state of the process */   
	msgbuf* msg_front;
	msgbuf* msg_last;	
} PCB;



#endif // ! K_RTX_H_
