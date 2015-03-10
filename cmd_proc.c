#include "cmd_proc.h"
#include "k_process.h"

#include "uart_polling.h"

void print_rpq_process() {
	int i;
	PCB** ready_queue = get_rpq();
	uart0_put_string("Ready queue processes: \n\r");
	for (i = 0; i < NUM_TEST_PROCS; i++) {
		if (ready_queue[i]->m_pid == NULL_PROC_ID) {
			return;
		}
		uart0_put_string("Process ");
		uart0_put_char('0'+ready_queue[i]->m_pid);
		uart0_put_string(": priority = ");
		uart0_put_char('0'+ready_queue[i]->m_priority);
		uart0_put_string("\n\r");
	}
}

void print_blk_on_mem_process() {
	int i;
	PCB** blocked_queue = get_bq();
	uart0_put_string("Processes blocked on memory: \n\r");
	for (i = 0; i < NUM_TEST_PROCS; i++) {
		if (blocked_queue[i] == NULL) {
			return;
		}
		if (blocked_queue[i]->m_state == BLK_ON_MEM) {
			uart0_put_string("Process ");
			uart0_put_char('0'+blocked_queue[i]->m_pid);
			uart0_put_string(": priority = ");
			uart0_put_char('0'+blocked_queue[i]->m_priority);
			uart0_put_string("\n\r");
		}
	}
}

void print_blk_on_msg_process() {
	int i;
	PCB** blocked_queue = get_bq();
	uart0_put_string("Processes blocked on receive: \n\r");
	for (i = 0; i < NUM_TEST_PROCS; i++) {
		if (blocked_queue[i] == NULL) {
			return;
		}
		if (blocked_queue[i]->m_state == BLK_ON_MSG) {
			uart0_put_string("Process ");
			uart0_put_char('0'+blocked_queue[i]->m_pid);
			uart0_put_string(": priority = ");
			uart0_put_char('0'+blocked_queue[i]->m_priority);
			uart0_put_string("\n\r");
		}
	}
}
