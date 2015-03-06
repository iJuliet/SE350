/**
 * @file:   k_process.c  
 * @brief:  process management C file
 * @author: G013
 * @date:   2014/02/05
 * NOTE: The example code shows one way of implementing context switching.
 *       The code only has minimal sanity check. There is no stack overflow check.
 *       The implementation assumes only two simple user processes and NO HARDWARE INTERRUPTS. 
 *       The purpose is to show how context switch could be done under stated assumptions. 
 *       These assumptions are not true in the required RTX Project!!!
 *       If you decide to use this piece of code, you need to understand the assumptions and
 *       the limitations. 
 */

#include <LPC17xx.h>
#include <system_LPC17xx.h>
#include "k_process.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* ----- Global Variables ----- */
PCB **gp_pcbs;                  /* array of pcbs */
PCB *gp_current_process = NULL; /* always point to the current RUN process */

/* process initialization table */
PROC_INIT g_proc_table[NUM_TEST_PROCS + 1];
extern PROC_INIT g_test_procs[NUM_TEST_PROCS + 1];

//queues
PCB* ready_queue[NUM_TEST_PROCS+1];
PCB* blocked_queue[NUM_TEST_PROCS];


/*typedef struct _queue_node{
	struct _queue_node* next;
	PCB* pcb;
} queue_node;

typedef struct _queue{
	queue_node *first;
	queue_node *last;
} queue;
*/

typedef struct _msgbuf {
	int sender_pid;
	int receiver_pid;
	struct _msgbuf* next;
	int mtype; /* user defined message type */
	char mtext[1]; /* body of the message */
} msgbuf;

/*	
queue **ready_queue;

void enqueue(PCB *pcb, queue* q) {
	queue_node* node;

	node->pcb = pcb;
	node->next = null;
	q->last->next = node;
}

PCB* dequeue(queue q) {
	//return the first element in the queue
	PCB* first = q->array[q->front];
	q->front += 1;
	q->size -= 1;
	return first;
}*/



/**
 * @biref: initialize all processes in the system
 * NOTE: We assume there are only two user processes in the system in this example.
 */
void process_init() 
{
	int i;
	U32 *sp;
  
        /* fill out the initialization table */
	set_test_procs();
	for ( i = 0; i < NUM_TEST_PROCS; i++ ) {
		g_proc_table[i].m_pid = g_test_procs[i].m_pid;
		g_proc_table[i].m_stack_size = g_test_procs[i].m_stack_size;
		g_proc_table[i].mpf_start_pc = g_test_procs[i].mpf_start_pc;
		g_proc_table[i].m_priority = g_test_procs[i].m_priority;
	}
	
	g_proc_table[NUM_TEST_PROCS].m_pid = 0;
	g_proc_table[NUM_TEST_PROCS].mpf_start_pc = &null_process;
	g_proc_table[NUM_TEST_PROCS].m_stack_size = 0;
	g_proc_table[NUM_TEST_PROCS].m_priority = 4;
  
	/* initilize exception stack frame (i.e. initial context) for each process */
	for ( i = 0; i < NUM_TEST_PROCS+1; i++ ) {
		int j;
		(gp_pcbs[i])->m_pid = (g_proc_table[i]).m_pid;
		(gp_pcbs[i])->m_priority = (g_proc_table[i]).m_priority;
		(gp_pcbs[i])->m_state = NEW;
		(gp_pcbs[i])->msg_front = NULL;
		(gp_pcbs[i])->msg_last = NULL;
		
		sp = alloc_stack((g_proc_table[i]).m_stack_size);
		*(--sp)  = INITIAL_xPSR;      // user process initial xPSR  
		*(--sp)  = (U32)((g_proc_table[i]).mpf_start_pc); // PC contains the entry point of the process
		for ( j = 0; j < 6; j++ ) { // R0-R3, R12 are cleared with 0
			*(--sp) = 0x0;
		}
		(gp_pcbs[i])->mp_sp = sp;
	}
	//organize priority queue
	for( i = 0; i < NUM_TEST_PROCS; i++) {
		rpq_enqueue(gp_pcbs[i]);
	}
	ready_queue[NUM_TEST_PROCS] = gp_pcbs[NUM_TEST_PROCS];
}

/*@brief: scheduler, pick the pid of the next to run process
 *@return: PCB pointer of the next to run process
 *         NULL if error happens
 *POST: if gp_current_process was NULL, then it gets set to pcbs[0].
 *      No other effect on other global variables.
 */

PCB *scheduler(void)
{
	PCB* oldProc;

	if(gp_current_process != NULL){
		rpq_enqueue(gp_current_process);
	}
	oldProc = gp_current_process;
	gp_current_process = rpq_dequeue();
	if(oldProc != NULL){
		process_switch(oldProc);
	}
	
	return gp_current_process;
}

/*@brief: switch out old pcb (p_pcb_old), run the new pcb (gp_current_process)
 *@param: p_pcb_old, the old pcb that was in RUN
 *@return: RTX_OK upon success
 *         RTX_ERR upon failure
 *PRE:  p_pcb_old and gp_current_process are pointing to valid PCBs.
 *POST: if gp_current_process was NULL, then it gets set to pcbs[0].
 *      No other effect on other global variables.
 */
int process_switch(PCB *p_pcb_old) 
{
	PROC_STATE_E state;
	
	state = gp_current_process->m_state;

	if (state == NEW) {
		if (gp_current_process != p_pcb_old && p_pcb_old->m_state != NEW) {
			p_pcb_old->m_state = RDY;
			p_pcb_old->mp_sp = (U32 *) __get_MSP();
		}
		gp_current_process->m_state = RUN;
		__set_MSP((U32) gp_current_process->mp_sp);
		__rte();  // pop exception stack frame from the stack for a new processes
	} 
	
	/* The following will only execute if the if block above is FALSE */

	if (gp_current_process != p_pcb_old) {
		if (state == RDY){ 
			p_pcb_old->m_state = RDY; 
			p_pcb_old->mp_sp = (U32 *) __get_MSP(); // save the old process's sp
			gp_current_process->m_state = RUN;
			__set_MSP((U32) gp_current_process->mp_sp); //switch to the new proc's stack    
		} else {
			gp_current_process = p_pcb_old; // revert back to the old proc on error
			return RTX_ERR;
		} 
	}
	return RTX_OK;
}
/**
 * @brief release_processor(). 
 * @return RTX_ERR on error and zero on success
 * POST: gp_current_process gets updated to next to run process
 */
int k_release_processor(void)
{
	PCB *p_pcb_old = NULL;
	
	p_pcb_old = gp_current_process;
	gp_current_process = scheduler();
	
	if ( gp_current_process == NULL  ) {
		gp_current_process = p_pcb_old; // revert back to the old process
		return RTX_ERR;
	}
  if ( p_pcb_old == NULL ) {
		p_pcb_old = gp_current_process;
	}
	return process_switch(p_pcb_old);
}

int k_set_process_priority(int process_id, int priority) {
	int i,j;
	int oldPriority = ready_queue[i]->m_priority;
	PCB* tmp;
	//set priority for specified process
	for (i = 0; i < NUM_TEST_PROCS; i++ ) {
		if (gp_pcbs[i]->m_pid == process_id) {
			gp_pcbs[i]->m_priority = priority;
		}
	}
	//reorder ready_queue
	for( i = 0; i < NUM_TEST_PROCS+1; i++) {
		if (ready_queue[i]->m_pid == process_id) {
			if(oldPriority == priority){
				return RTX_OK;
			}else if(oldPriority > priority){
				//priority is higher
				//move forward
				tmp = ready_queue[i];
				j = i;
				while(j > 0 && ready_queue[j-1]->m_priority > priority){
					j--;
					ready_queue[j+1] = ready_queue[j];
				}
				ready_queue[j] = tmp;
			}else{
				//priority is lower
				//move backward
				tmp = ready_queue[i];
				j = i;
				while(j < NUM_TEST_PROCS && ready_queue[j+1]->m_priority <= priority) {
					ready_queue[j] = ready_queue[j+1];
					j++;
				}
				ready_queue[j]=tmp;
			}
			return k_release_processor();
		}
	}
	return RTX_ERR;
}

int k_get_process_priority(int process_id) {
	int i;
	for (i = 0; i < NUM_TEST_PROCS; i++ ) {
		if (gp_pcbs[i]->m_pid == process_id) {
			return gp_pcbs[i]->m_priority;
		}
	}
	return RTX_ERR;
}

/**
 * @brief: a process that does nothing but 
 * releasing processor repetitively
 */
void null_process(void) {
	while (1) {
		k_release_processor();
	}
}


//add to the right place in ready_queue
void rpq_enqueue(PCB* pcb) {
	int i,j;
	if(pcb -> m_state == BLK_ON_MEM){
		return;
	}
	for(i = 0; i < NUM_TEST_PROCS; i++) {
		if(pcb->m_pid == ready_queue[i]->m_pid){
			//it is in the queue already
			return;
		}
		if(pcb->m_priority < ready_queue[i]->m_priority) {
			for(j = NUM_TEST_PROCS; j > i; j--){
				ready_queue[j] = ready_queue[j-1]; 
			}
			ready_queue[i] = pcb;
			return;
		}
	}
}


PCB* rpq_dequeue() {
	int i;
	PCB* tmp;
	tmp = ready_queue[0];
	for(i = 0; i < NUM_TEST_PROCS; i++) {
			ready_queue[i] = ready_queue[i+1];
	}
	
	return tmp;
}

void bq_enqueue(PCB* pcb) {
	int i;
	for (i = 0; i < NUM_TEST_PROCS; i++) {
		if (blocked_queue[i] == NULL) {
			blocked_queue[i] = pcb;
			return;
		}
	}
}

PCB* bq_dequeue(void) {
	int i;
	PCB* tmp;
	for (i = 0; i < NUM_TEST_PROCS; i++) {
		if (blocked_queue[i] != NULL) {
			tmp = blocked_queue[i];
			blocked_queue[i] = NULL;
			return tmp;
		}
	}
	return NULL;
}

int msg_enqueue(PCB* pcb, void* msg_envelope){
	if (pcb == NULL || msg_envelope == NULL) {
		return RTX_ERR;
	}
	if (pcb->msg_front == NULL){
		pcb->msg_front = msg_envelope;
		pcb->msg_last = msg_envelope;
	}
	else{
		((msgbuf*)pcb->msg_last)->next = msg_envelope;
		pcb->msg_last = msg_envelope;
	}
	return RTX_OK;
}

void* msg_dequeue(PCB* pcb, int sender_pid){
	msgbuf* prev;	
	msgbuf* temp = (msgbuf*)pcb->msg_front;		
	while (temp->sender_pid != sender_pid && temp != NULL) {
		prev = temp;
		temp = temp->next;
	}
	if(temp == NULL){
		//block the current process
	} else {
		if (pcb->msg_front == temp) {
			pcb->msg_front = temp->next;
		} else {
			prev->next = temp->next;
		}
		if (pcb->msg_last == pcb->msg_front) {
			pcb->msg_last = NULL;
		} else if (pcb->msg_last == temp) {
			pcb->msg_last = prev;
		}
	}
	return temp;
}

PCB* get_current_proc(void) {
	return gp_current_process;
}

PCB* get_pcb_from_pid(int process_id){
	int i = 0;
	for (; i < NUM_TEST_PROCS; i++ ) {
		if (gp_pcbs[i]->m_pid == process_id) {
			return gp_pcbs[i];
		}
	}
	return NULL;
}

//send message
int k_send_message(int process_id, void *message_envelope){
	int status;
	PCB* receiving_proc = get_pcb_from_pid(process_id);
	//enqueue env onto msg_queue of receiving proc
	status = msg_enqueue(receiving_proc, message_envelope);
	if (receiving_proc->m_state == BLK_ON_MSG) {
		receiving_proc->m_state = RDY;
		rpq_enqueue(receiving_proc);
	}
	return status;
}

//receive message
void* k_receive_message(int sender_id) {
	// atomic(on)
	void* env;
	PCB* curr_proc = get_current_proc();
	while(curr_proc->msg_front == NULL) {
		curr_proc->m_state = BLK_ON_MSG;
		k_release_processor();
	}
	env = msg_dequeue(curr_proc, sender_id);
	// atomic(off)
	return env;
}
