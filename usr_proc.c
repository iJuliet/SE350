/**
 * @file:   usr_proc.c
 * @brief:  Two user processes: proc1 and proc2
 * @author: Yiqing Huang
 * @date:   2014/01/17
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 */
 
 /*
 
 User-side Tests:
 
 Test 1: Test our set_process_priority and get_process_priority API call
 
 Test 2: Test if our OS would pick the process of the highest priority
 
 Test 3: Successfully request a memory block
 
 Test 4: Successfully release a valid memory block
 
 Test 5: Return RTX_ERR when trying to release a invalid memory block
 
 Test 6: OS keeps running the process of the highest priority
 
 Test 7: OS picks the first process in the ready queue that has the highest priority
 
 Test 8: when there is no free memory, it will call release_processor() immediately
 
 */

#include "uart_polling.h"
#include "usr_proc.h"
#include "wall_clock.h"
#include "set_priority_proc.h"
#include "string.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */



/* initialization table item */
PROC_INIT g_test_procs[NUM_USER_PROCS];
int test_num; // this indicates which test are we on
int ok_tests;

void set_test_procs() {
	int i;
	test_num = 1;
	ok_tests = 0;
	//uart0_put_string("setting test procs");
	for( i = 0; i < NUM_USER_PROCS; i++ ) {
		g_test_procs[i].m_pid=(U32)(i+1);
		g_test_procs[i].m_priority=LOWEST;
		g_test_procs[i].m_stack_size=0x100;
	}
  
	g_test_procs[0].mpf_start_pc = &proc1;
	g_test_procs[1].mpf_start_pc = &proc2;
	g_test_procs[2].mpf_start_pc = &proc3;
	g_test_procs[3].mpf_start_pc = &proc4;
	g_test_procs[4].mpf_start_pc = &proc5;
	g_test_procs[5].mpf_start_pc = &proc6;
	
	//TODO: Process A, B and C
	
	g_test_procs[6].mpf_start_pc = &proc_a;
	g_test_procs[6].m_priority = MEDIUM;
	g_test_procs[7].mpf_start_pc = &proc_b;
	g_test_procs[7].m_priority = HIGH;
	g_test_procs[8].mpf_start_pc = &proc_c;
	g_test_procs[8].m_priority = HIGH;
	
	g_test_procs[9].mpf_start_pc = &set_priority_process;
	g_test_procs[9].m_priority = HIGH;
	
	g_test_procs[10].mpf_start_pc = &wc_process;
	g_test_procs[10].m_priority = HIGH;
	
	g_test_procs[3].m_priority=HIGH;
	g_test_procs[2].m_priority=HIGH;

}

/**
 * @brief: a process that prints five uppercase letters
 *         and then yields the cpu.
 */
// proc1 receive proc
void proc1(void) 
{
	MSGBUF* message;
	int received_messages = 0;	
	
	uart0_put_string("G013_test: START\n\r");
	uart0_put_string("G013_test: total 6 tests\n\r");
	
	while(1){
		
		if (received_messages < 20){
				set_process_priority(2,MEDIUM);
				message = (MSGBUF*)receive_message(NULL);
				received_messages++;
				uart0_put_string("Proc1 received message from Proc2!\n\r");
				//uart0_put_char('0'+received_messages);
				if (received_messages == 1) {
					uart0_put_string("G013_test: test 2 pass\n\r");
					ok_tests++;
					test_num++;
				}
				if (received_messages == 10) {
					uart0_put_string("G013_test: test 3 pass\n\r");
					ok_tests++;
					test_num++;
				}
				if (received_messages == 11) {
					uart0_put_string("G013_test: test 4 pass\n\r");
					ok_tests++;
					test_num++;
				}
				if (received_messages == 20) {
					uart0_put_string("G013_test: test 5 pass\n\r");
					ok_tests++;
					test_num++;
					set_process_priority(1,LOW);
					set_process_priority(3,MEDIUM);
				}
				release_memory_block(message);
		}
		
		release_processor();
	}
}

/**
 * @brief: a process that prints five numbers
 *         and then yields the cpu.
 */
// proc 2 send message
void proc2(void)
{
	
	MSGBUF* msg_env;
	int sent_msg = 0;
	
	while(1){
		
		if (test_num == 1) {
			uart0_put_string("G013_test: test 1 pass\n\r");
			test_num++;
			ok_tests++;
		}
		
		if(sent_msg < 10){
			msg_env = (MSGBUF *)request_memory_block();
			msg_env->mtype = 0;
			msg_env->mtext[0] = 't';
			
			set_process_priority(1,HIGH);
			uart0_put_string("Proc2 sending message to Proc1...\n\r");
			send_message(1,msg_env);
			sent_msg++;
		} else if(sent_msg < 20){
			uart0_put_string("Proc2 sending delayed message to Proc1...\n\r");
			msg_env = (MSGBUF *)request_memory_block();
			msg_env->mtype = 0;
			msg_env->mtext[0] = 't';
			set_process_priority(1,HIGH);
			delayed_send(1,msg_env,1000);
			sent_msg++;
		}

		release_processor();
	}
}

/**
 * @brief: a process that request a memory block
 *        
 */
void proc3(void)
{
	MSGBUF* msg;
	msg = request_memory_block();
	msg->mtype = KCD_REG;
	msg->mtext[0] = '%';
	msg->mtext[1] = 'o';
	msg->mtext[2] = '\0';
	send_message(KCD_PROC_ID,msg);
	while(1){
		//uart0_put_string("Please enter the command \"%o\"\n\r");
		msg = (MSGBUF*)receive_message(NULL);
		if (msg->mtype == DEFAULT) {
			uart0_put_string("Proc3 received a command mesage: ");
			uart0_put_string(msg->mtext);
			uart0_put_string("\n\r");
			uart0_put_string("G013_test: test 6 pass\n\r");
			ok_tests++;
			printEndTestString();
		}
		//set_process_priority(3,LOWEST);
		release_processor();
	}
}


void proc4(void)
{
	
	MSGBUF* msg;
	int i, ret_val;
	msg = request_memory_block();
	msg->mtype = KCD_REG;
	msg->mtext[0] = '%';
	msg->mtext[1] = 'M';
	msg->mtext[2] = '\0';
	send_message(KCD_PROC_ID,msg);
	while(1){
		uart1_put_string("proc4---");
		msg = (MSGBUF*)receive_message(NULL);
		if (msg->mtype == DEFAULT) {
			for(i=0; i< 2000;++i){
				 request_memory_block();
				//lead to memory leak
			}
		}
		release_processor();
	}
}

void proc6(void){
	 while(1){
			release_processor();
	 }
}

void proc5(void){
	 while(1){
			release_processor();
	 }
}
void proc_a(void){
	MSGBUF* msg;
	int i, temp_num;
	msg = request_memory_block();
	msg->mtype = KCD_REG;
	msg->mtext[0] = '%';
	msg->mtext[1] = 'Z';
	msg->mtext[2] = '\0';
	send_message(KCD_PROC_ID,msg);
	while(1){
		msg = (MSGBUF*)receive_message(NULL);
		if (msg->mtype == DEFAULT) {
			//check if data is %Z
			if(msg->mtext[0] == '%' && msg->mtext[1] == 'Z'){
				release_memory_block(msg);
				break;
			}
		}else{
			release_memory_block(msg);
		}
	}
	while(1){
		int num_digits, index;
		num_digits = 0;
		index = 0;
		msg = request_memory_block();
		msg->mtype = COUNT_REPORT;
		temp_num = i;
		//count digits
		while(temp_num != 0){
			num_digits++;
			temp_num = temp_num/10;
		}
		
		temp_num = i;
		msg->mtext[num_digits] = '\0';
		while(num_digits != 0){
			 msg->mtext[num_digits-1] = temp_num%10 + '0';
			 temp_num = temp_num/10;
			 num_digits--;
		}
		send_message(PROC_B_ID, msg);
		i++;
		release_processor();
	}
}


void proc_b(void){
	 MSGBUF* msg;
	while(1){
		msg = (MSGBUF*)receive_message(NULL);
		send_message(PROC_C_ID, msg);
		release_processor();
	}
}
void proc_c(void){
	 MSG_QUEUE_NODE* msg_queue = NULL;
	 MSG_QUEUE_NODE* msg_queue_end = NULL;
	 MSGBUF* msg;
	
	 while(1){
		 if(msg_queue == NULL){
			 msg = receive_message(NULL);
		 }
		 else{
			 msg = (MSGBUF *)dequeue(msg_queue, msg_queue_end);
		 }
		 if(msg->mtype == COUNT_REPORT){
			 int num, index, digit;
			  while(msg->mtext[index] != '\0'){
					digit = msg->mtext[index] - '0';
					num = num*10 + digit;
					index++;
				}
				if(num%20 == 0){
					
					MSGBUF* delayed_msg;
					char text[] = "PROCESS C";
					msg->mtype = CRT_REQ;
					strncpy(msg->mtext, text, MAX_MSG_SIZE);
					send_message(CRT_PROC_ID, msg);
					//hibernate 10s
					delayed_msg = (MSGBUF* )request_memory_block();
					delayed_msg -> mtype = WAKE_UP_TEN;
					delayed_msg -> mtext[0] = '\0';
					delayed_send(PROC_C_ID, delayed_msg, 10000);
					while(1){
						 msg = receive_message(NULL);
						 if(msg->mtype == WAKE_UP_TEN){
							  release_memory_block(msg);
							  break;
						 }else{
							  enqueue(msg_queue, msg_queue_end, (MSG_QUEUE_NODE*)msg);
						 }
					}

				}else{
					release_memory_block(msg);
				}
		 }
			release_processor();
	 }
}

MSG_QUEUE_NODE* dequeue(MSG_QUEUE_NODE* queue_start, MSG_QUEUE_NODE* queue_end){
	MSG_QUEUE_NODE* temp;
	if(queue_start == NULL){
		return NULL;
	}else{
		if(queue_start == queue_end){
			queue_end = NULL;
		}
		temp = queue_start;
		queue_start = queue_start->next;
		return temp;
	}
}

void enqueue(MSG_QUEUE_NODE* queue_start, MSG_QUEUE_NODE* queue_end, MSG_QUEUE_NODE* new_node){
		if(queue_start == NULL){
			queue_start = new_node;
			queue_end = new_node;
		}else{
			new_node->next = NULL;
			queue_end -> next = new_node;
			queue_end = new_node;
		}
}



void printEndTestString(){
		uart0_put_string("G013_test: ");
		uart0_put_char('0'+ok_tests%10);
		uart0_put_char('/');
		uart0_put_char('0'+test_num%10);
		uart0_put_string(" test OK\n\r");
		uart0_put_string("G013_test: ");
		uart0_put_char('0'+(test_num-ok_tests)%10);
		uart0_put_char('/');
		uart0_put_char('0'+test_num%10);
		uart0_put_string(" test FAIL\n\r");
	  uart0_put_string("G013_test: END\n\r");
}
