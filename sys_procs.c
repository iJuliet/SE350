
#include <LPC17xx.h>
#include <system_LPC17xx.h>
#include "sys_procs.h"

#include "string.h"
#include "uart.h"
#include "uart_polling.h"
#include "k_rtx.h"

#define KCD_PROCESS 6
#define CRT_PROCESS 7
#define KCD_PROC_ID 12
#define CRT_PROC_ID 13


#define DEFAULT 0
#define KCD_REG 1
#define CRT_REQ 2

#define UART_PROC_ID 15

#define MAX_MSG_SIZE 64

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

typedef struct _kcdcmd {
    int pid;
    char cmd[10];
} kcdcmd;
kcdcmd commands [10];
int regCmds = 0;

//helper function
void clear_buffer(char* buffer){
		int m;
		for(m=0; m <MAX_MSG_SIZE; m++){
			buffer[m] = '\0';
		}
}
void set_up_sys_procs(PROC_INIT *g_proc_table){
	//set up for kcd-process
	g_proc_table[KCD_PROCESS].m_pid = KCD_PROC_ID;
	g_proc_table[KCD_PROCESS].mpf_start_pc = &kcd_process;
	g_proc_table[KCD_PROCESS].m_stack_size = 0x200;
	g_proc_table[KCD_PROCESS].m_priority = SYSTEM; 
	
	//set up for crt-process
	g_proc_table[CRT_PROCESS].m_pid = CRT_PROC_ID;
	g_proc_table[CRT_PROCESS].mpf_start_pc = &crt_process;
	g_proc_table[CRT_PROCESS].m_stack_size = 0x100;
	g_proc_table[CRT_PROCESS].m_priority = SYSTEM; 
}

void crt_process(){
	
	msgbuf* msg_env;
	while(1){

		msg_env = (msgbuf*)receive_message(NULL);
		
			if (msg_env == NULL || msg_env->mtype != CRT_REQ) {
					// wrong message
					release_memory_block(msg_env);
			} else {
					// forwards the message to uart_i_process
					send_message(UART_PROC_ID, msg_env);
					set_uart0_interrupt();
			}
		//release_processor();
	}
	
}

void kcd_process(){
	
	msgbuf* msg_env, *msg_for_cmd;
	int validCmd;
	char msgText [2];
	char* temp;
	char buffer[MAX_MSG_SIZE];
	int pid;
	int i,j;
	buffer[0] = '\0';
	validCmd = 0;
	clear_buffer(buffer);
	i=0;
	while(1) {

			msg_env = (msgbuf* )receive_message(NULL);

		
		//determine message type
		if(msg_env->mtype == DEFAULT){
				strncpy(msgText, msg_env->mtext, strlen(msg_env->mtext));
				temp = msgText;
				if(validCmd){
						//read chars until '\r'
						if(*temp == '\r'){
							msg_for_cmd = (msgbuf* )request_memory_block();
							strncpy(msg_for_cmd->mtext, buffer, MAX_MSG_SIZE);
							msg_for_cmd->mtype = DEFAULT;
							validCmd = 0;
							clear_buffer(buffer);
							i = 0;
							msg_env->mtext[0] = '\n';
							msg_env->mtext[1] = '\r';
							msg_env->mtext[2] = '\0';
							//sent to crt
							msg_env->mtype = CRT_REQ;
							send_message(CRT_PROC_ID, msg_env);
							send_message(pid, msg_for_cmd);
						}else{
							//release the msg
							//sent to crt
							buffer[i++] = *temp;
							msg_env->mtype = CRT_REQ;
							send_message(CRT_PROC_ID, msg_env);
							
							
						}
					
						
				}else if((msgText[0] == '%' && buffer[0] == '\0') || buffer[0] == '%'){
					
						//check if cmd is completed
						if( *temp == ' ' || *temp == '\r'){
							for (j = 0; j < regCmds; j++) {
								if (strcmp(commands[j].cmd, buffer) == 0) {
										pid = commands[j].pid;
										if( *temp == '\r'){
											//input completed
											msg_for_cmd = (msgbuf*)request_memory_block();
											strncpy(msg_for_cmd->mtext, buffer, MAX_MSG_SIZE);
											msg_for_cmd->mtype = DEFAULT;
											clear_buffer(buffer);
											msgText[0] = '\0'; // clears both buffers, kinda hacky, more testing needed
											validCmd = 0;
											i = 0;
											//sent to crt
											msg_env->mtype = CRT_REQ;
											msg_env->mtext[0] = '\n';
											msg_env->mtext[1] = '\r';
											msg_env->mtext[2] = '\0';
											send_message(CRT_PROC_ID, msg_env);
											
											
											send_message(pid, msg_for_cmd);
											goto DONE;
										}else{
											buffer[i++] = *temp;
											validCmd = 1;
											msg_env->mtype = CRT_REQ;
											send_message(CRT_PROC_ID, msg_env);
											goto DONE;
										}
								}
							}
							
							
							//invalid command
							
							//read chars until '\r'
							if(*temp == '\r'){
								msg_env->mtext[0] = '\n';
								msg_env->mtext[1] = '\r';
								msg_env->mtext[2] = '\0';
							}
							
							//reset
							validCmd = 0;
							clear_buffer(buffer);
							i = 0;
							msgText[0] = '\0'; // clears both buffers, kinda hacky, more testing needed
							
							//sent to crt
							msg_env->mtype = CRT_REQ;
							send_message(CRT_PROC_ID, msg_env);
						
						
						} else {
							//waiting for more chars and release the block
							buffer[i++] = *temp;
							//release_memory_block(msg_env);
							
							msg_env->mtype = CRT_REQ;
							send_message(CRT_PROC_ID, msg_env);
						}
						
						/*
						while (temp!= NULL && *temp != ' ' && *temp != '\r') {
								if (*temp != '\0') {
										buffer[i++] = *temp++;
								} else {
										*temp++;
								}
						}*/
					
				}else{
					//send msg to crt_process
					if(*temp == '\r'){
							msg_env->mtext[0] = '\n';
							msg_env->mtext[1] = '\r';
							msg_env->mtext[2] = '\0';
					}
					msg_env->mtype = CRT_REQ;
					send_message(CRT_PROC_ID, msg_env);
				}
		} else if (msg_env->mtype == KCD_REG) {
			
				if ( regCmds < 10 ) {
						commands[regCmds].pid = msg_env->sender_pid;
						strncpy(commands[regCmds].cmd, msg_env->mtext, strlen(msg_env->mtext));
						regCmds++;
				} else {
						// Reaches maximum of commands that can be registered
				}
				release_memory_block(msg_env);
		}
		DONE:
			continue;
		//release_processor();
	}
}

