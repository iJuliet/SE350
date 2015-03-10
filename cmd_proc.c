#include "k_process.h"
/*
void print_rpq_process() {
	int i;
	uart0_put_string("Ready queue processes: \n\r");
	for (i = 0; i < NUM_TEST_PROCS; i++) {
		if (read_queue[i]->m_pid == NULL_PROC_ID) {
			return;
		}
		uart0_put_string("Process ");
		uart0_put_char('0'+ready_queue[i]->m_pid);
		uart0_put_string(": priority = ");
		uart0_put_char('0'+ready_queue[i]->m_priority;
		uart0_put_string("\n\r");
	}
}

void print_blk_on_mem_process() {
	int i;
	uart0_put_string("Processes blocked on memory: \n\r");
}
	*/