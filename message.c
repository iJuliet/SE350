#include <LPC17xx.h>
#include <system_LPC17xx.h>
#include "message.h"
#include "k_rtx.h"

extern int current_time;
extern msgbuf* timeout_queue;
extern PCB **gp_pcbs;


int msg_enqueue(PCB* pcb, msgbuf* msg_envelope){
	if (pcb == NULL || msg_envelope == NULL) {
		return RTX_ERR;
	}
	if (pcb->msg_front == NULL){
		pcb->msg_front = msg_envelope;
		pcb->msg_last = msg_envelope;
	}
	else{
		(pcb->msg_last)->next = msg_envelope;
		pcb->msg_last = msg_envelope;
	}
	return RTX_OK;
}

void* msg_dequeue(PCB* pcb, int* sender_pid){
	msgbuf* prev;	
	msgbuf* temp = pcb->msg_front;
	if(sender_pid == NULL){
			if (pcb->msg_last == pcb->msg_front) {
				pcb->msg_last = NULL;
			}
			pcb->msg_front = pcb ->msg_front -> next;
			return temp;
	}
	while (temp->sender_pid != *sender_pid && temp != NULL) {
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


//send message
int k_send_message(int process_id, void *env){
	int status;
	msgbuf* message_envelope;
	PCB* timer;
	PCB* receiving_proc = get_pcb_from_pid(process_id);
	
	
	__disable_irq();
	//set the casted message_envelope
	message_envelope = (msgbuf*) env;
	message_envelope -> sender_pid = (get_current_proc()) -> m_pid;
	message_envelope -> receiver_pid = process_id;
	message_envelope -> send_time = current_time;
	message_envelope -> next = NULL;
	
	//enqueue env onto msg_queue of receiving proc
	status = msg_enqueue(receiving_proc, message_envelope);
	if (receiving_proc->m_state == BLK_ON_MSG) {
		receiving_proc->m_state = RDY;
		bq_dequeue_by_pid(receiving_proc->m_pid);
		rpq_enqueue(receiving_proc);
		timer = gp_pcbs[TIMER_I_PROCESS];
		// preemption
		if (receiving_proc->m_priority <= get_current_proc()->m_priority) {
				__enable_irq();
				k_release_processor();
				__disable_irq();
		}
	}
	__enable_irq();
	return status;
}

//send message with no preemption
int k_send_message_no_preemp(int process_id, void *env){
	int status;
	msgbuf* message_envelope;
	PCB* timer;
	PCB* receiving_proc = get_pcb_from_pid(process_id);
	
	
	__disable_irq();
	//set the casted message_envelope
	message_envelope = (msgbuf*) env;
	message_envelope -> sender_pid = (get_current_proc()) -> m_pid;
	message_envelope -> receiver_pid = process_id;
	message_envelope -> send_time = current_time;
	message_envelope -> next = NULL;
	
	//enqueue env onto msg_queue of receiving proc
	status = msg_enqueue(receiving_proc, message_envelope);
	if (receiving_proc->m_state == BLK_ON_MSG) {
		receiving_proc->m_state = RDY;
		bq_dequeue_by_pid(receiving_proc->m_pid);
		rpq_enqueue(receiving_proc);
		timer = gp_pcbs[TIMER_I_PROCESS];
	}
	__enable_irq();
	return status;
}


//receive message
void* k_receive_message(int* sender_id) {
	// atomic(on)
	msgbuf* env;
	PCB* curr_proc = get_current_proc();
	__disable_irq();
	while(curr_proc->msg_front == NULL) {
		__enable_irq();
		curr_proc->m_state = BLK_ON_MSG;
		bq_enqueue(curr_proc);
		k_release_processor();
		__disable_irq();
	}
	env = msg_dequeue(curr_proc, sender_id);
	// atomic(off)
	__enable_irq();
	return (void*)env;
}

int k_delayed_send(int process_id, void *env, int delay){
		
		msgbuf* temp, *prev, *message_envelope;
		
		int send_time = delay + current_time;
		//set the casted message_envelope
		message_envelope = (msgbuf *) env;
	
		message_envelope -> sender_pid = get_current_proc() -> m_pid;
		message_envelope -> receiver_pid = process_id;
		message_envelope -> send_time = send_time;
		message_envelope -> next = NULL;
	
		//enqueue the msg to timeout_queue
		//the queue should always remain sorted
		if(timeout_queue == NULL){
			 timeout_queue = message_envelope;
		}
		else{
			temp = timeout_queue -> next;
			prev = timeout_queue;
			 while(temp != NULL && temp->send_time < send_time){
					prev = temp;
					temp = temp ->next;
			 }
			 //should be insert here
			 prev->next = message_envelope;
		}
		return RTX_OK;
}
