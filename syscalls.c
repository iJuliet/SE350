
#include <LPC17xx.h>
#include <system_LPC17xx.h>
#include "syscalls.h"

#include "string.h"
#include "uart.h"
#include "uart_polling.h"
//#include "k_rtx.h"

#define KCD_PROCESS 5
#define CRT_PROCESS 6
#define KCD_PROC_ID 12
#define CRT_PROC_ID 13


#define DEFAULT 0
#define KCD_REG 1
#define CRT_REQ 2

#define UART_PROC_ID 15

#define MAX_MSG_SIZE 128

typedef struct _kcdcmd {
    int pid;
    char cmd[10];
} kcdcmd;
kcdcmd commands [10];
int regCmds = 0;


void set_up_sys_procs(PROC_INIT *g_proc_table){
	//set up for kcd-process
	g_proc_table[KCD_PROCESS].m_pid = KCD_PROC_ID;
	g_proc_table[KCD_PROCESS].mpf_start_pc = &kcd_process;
	g_proc_table[KCD_PROCESS].m_stack_size = 0x100;
	g_proc_table[KCD_PROCESS].m_priority = 0; 
	
	//set up for crt-process
	g_proc_table[CRT_PROCESS].m_pid = CRT_PROC_ID;
	g_proc_table[CRT_PROCESS].mpf_start_pc = &crt_process;
	g_proc_table[CRT_PROCESS].m_stack_size = 0x100;
	g_proc_table[CRT_PROCESS].m_priority = 0; 
}

void crt_process(){
	MSGBUF* msg_env;
	LPC_UART_TypeDef* pUart;
	while(1){
		msg_env = (MSGBUF*)receive_message(NULL);
			if (msg_env == NULL || msg_env->mtype != CRT_REQ) {
					// wrong message
					release_memory_block(msg_env);
			} else {
					// forwards the message to uart_i_process
					send_message(UART_PROC_ID,msg_env);
					pUart = (LPC_UART_TypeDef*)LPC_UART0;
					pUart->IER |= IER_THRE;
					pUart->THR = '\0';
			}
	}
}

void kcd_process(){
	MSGBUF* msg_env;
	char msgText [MAX_MSG_SIZE];
	char* temp;
	char buffer[10];
	int i = 0;
	while(1) {
			msg_env = (MSGBUF* )receive_message(NULL);
			//determine message type
			if(msg_env->mtype == DEFAULT){
					strncpy(msgText, msg_env->mtext, strlen(msg_env->mtext));
					temp = msgText;
					if (msgText[0] == '%') {
							while (*temp != ' ' && *temp != '\r' && i < 10) {
									if (*temp != '\0') {
											buffer[i++] = *temp++;
									} else {
											temp++; //*temp++ maybe? need more testing
									}
							}
							
							if (i >= 10) { // command identifier should have length < 10
									send_message(CRT_PROC_ID, msg_env);
							} else {
									for (i = 0; i < regCmds; i++) {
											if (strcmp(commands[i].cmd, buffer) == 0) {
													send_message(commands[i].pid, msg_env);
													buffer[0] = '\0';
													msgText[0] = '\0'; // clears both buffers, kinda hacky, more testing needed
													goto DONE;
											}
									}
									
									//send msg to crt_process
									send_message(13, msg_env);
							}
					DONE:
							continue;
					}
			} else if (msg_env->mtype == KCD_REG) {
					if ( regCmds < 10 ) {
							//commands[regCmds].pid = msg_env->sender_pid;
							strncpy(commands[regCmds].cmd, msg_env->mtext, strlen(msg_env->mtext));
							regCmds++;
					} else {
							// Reaches maximum of commands that can be registered
					}
					release_memory_block(msg_env);
			}
	}
}

