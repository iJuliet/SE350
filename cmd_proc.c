#include "cmd_proc.h"
#include "k_process.h"

#include "printf.h"

#include "uart_polling.h"

void print_rpq_process() {
	int i, pid;
	PCB** ready_queue = get_rpq();
	uart1_put_string("Ready queue processes: \n\r");
	for (i = 0; i < NUM_TEST_PROCS; i++) {
		if (ready_queue[i]->m_pid == NULL_PROC_ID) {
			return;
		}
		printf("Process %d: priority = %d \n\r", ready_queue[i]->m_pid, ready_queue[i]->m_priority);
		uart1_put_string("Process ");
		pid = ready_queue[i]->m_pid;
		if(pid >10){
			uart1_put_char('0'+pid/10);
		}
		uart1_put_char('0'+pid%10);
		uart1_put_string(": priority = ");
		uart1_put_char('0'+ready_queue[i]->m_priority);
		uart1_put_string("\n\r");
	}
}

void print_blk_on_mem_process() {
	int i, pid;
	PCB** blocked_queue = get_bq();
	uart1_put_string("Processes blocked on memory: \n\r");
	for (i = 0; i < NUM_TEST_PROCS; i++) {
		if (blocked_queue[i] == NULL) {
			return;
		}
		if (blocked_queue[i]->m_state == BLK_ON_MEM) {
			//printf("Process %d: priority = %d \n\r", blocked_queue[i]->m_pid, blocked_queue[i]->m_priority);
			uart1_put_string("Process ");
			pid = blocked_queue[i]->m_pid;
			if(pid >10){
				uart1_put_char('0'+pid/10);
			}
			uart1_put_char('0'+pid%10);
			uart1_put_string(": priority = ");
			uart1_put_char('0'+blocked_queue[i]->m_priority);
			uart1_put_string("\n\r");
		}
	}
}

void print_blk_on_msg_process() {
	int i, pid;
	PCB** blocked_queue = get_bq();
	uart1_put_string("Processes blocked on receive: \n\r");
	for (i = 0; i < NUM_TEST_PROCS; i++) {
		if (blocked_queue[i] == NULL) {
			return;
		}
		if (blocked_queue[i]->m_state == BLK_ON_MSG) {
			//printf("Process %d: priority = %d \n\r", blocked_queue[i]->m_pid, blocked_queue[i]->m_priority);
			uart1_put_string("Process ");
			pid = blocked_queue[i]->m_pid;
			if(pid >10){
				uart1_put_char('0'+pid/10);
			}
			uart1_put_char('0'+pid%10);
			uart1_put_string(": priority = ");
			uart1_put_char('0'+blocked_queue[i]->m_priority);
			uart1_put_string("\n\r");
		}
	}
}
