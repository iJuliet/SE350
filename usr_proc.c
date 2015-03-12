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

#include "rtx.h"
#include "uart_polling.h"
#include "usr_proc.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

#define DEFAULT 0
#define KCD_REG 1
#define KCD_PROC_ID 12

/* initialization table item */
PROC_INIT g_test_procs[NUM_TEST_PROCS];
int test_num; // this indicates which test are we on
int ok_tests;

void set_test_procs() {
	int i;
	test_num = 1;
	ok_tests = 0;
	uart0_put_string("setting test procs");
	for( i = 0; i < NUM_TEST_PROCS; i++ ) {
		g_test_procs[i].m_pid=(U32)(i+1);
		g_test_procs[i].m_priority=LOWEST;
		g_test_procs[i].m_stack_size=0x100;
	}
  
	g_test_procs[0].mpf_start_pc = &proc1;
	g_test_procs[1].mpf_start_pc = &proc2;
	g_test_procs[2].mpf_start_pc = &proc3;
	g_test_procs[3].mpf_start_pc = &proc4;
	
	//g_test_procs[3].m_priority = MEDIUM;
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
	
	uart0_put_string("G013_test: START");
	uart0_put_string("G013_test: total 100 tests");
	/*G099_test: test 1 OK
	G099_test: test 2 OK
	G099_test: test 3 FAIL
	G099_test: 2/3 tests OK
	G099_test: 1/3 tests FAIL*/
	
	while(1){
		//set_process_priority(5,MEDIUM);
		/*
		//uart0_put_char('0'+received_messages%10);
		//uart0_put_string(" entering process 1\n\r");
		*/
		if(test_num == 1){
			if (received_messages < 21){
				message = (MSGBUF*)receive_message(NULL);
				received_messages++;
				uart0_put_string("Proc1 received message: ");
				uart0_put_string(message->mtext);
				release_memory_block(message);
			} else {
				uart0_put_string("G013_test: test 2 OK\n\r");
				ok_tests++;
				test_num++;
			}
		}
		else if(test_num > 1 && received_messages < 21){
				uart0_put_string("G013_test: test 1 fail\n\r");
				test_num++;
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
	void* temp;
	int sent_msg = 0;
	
	while(1){
		//uart0_put_char('0'+sent_msg%10);
		//uart0_put_string(" entering proc2\n\r");
		if (test_num == 1) {
			uart0_put_string("G013_test: test 1 pass\n\r");
			test_num++;
			ok_tests++;
		}
		
		if(sent_msg < 20){
			msg_env = (MSGBUF *)request_memory_block();
			msg_env->mtype = 0;
			msg_env->mtext[0] = 't';
			
			set_process_priority(1,HIGH);
			send_message(1,msg_env);
			sent_msg++;
		}
		else if(sent_msg == 20){
			uart0_put_string(" sending delayed message\n\r");
			msg_env = (MSGBUF *)request_memory_block();
			msg_env->mtype = 0;
			msg_env->mtext[0] = 't';
			set_process_priority(1,HIGH);
			k_delayed_send(1,msg_env,10);
			set_process_priority(3,HIGH);
			sent_msg++;
		}else{
			
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
	int status;
	MSGBUF* msg_env;
	
	while(1){
		/*
		//uart0_put_string("entering proc3\n\r");
		msg_env = (MSGBUF *)request_memory_block();
		msg_env->mtype = 0;
		msg_env->mtext[0] = 'b';
		set_process_priority(1,HIGH);
		uart0_put_string(" sending delayed message from proc 3\n\r");
		k_delayed_send(1,msg_env,2);*/
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
	msg->mtext[1] = 'o';
	msg->mtext[2] = '\0';
	send_message(KCD_PROC_ID,msg);
	//set_process_priority(5,LOW);
	while(1){
		msg = (MSGBUF*)receive_message(NULL);
		if (msg->mtype == DEFAULT) {
			uart0_put_string("Proc4 received a command mesage:\n\r");
			uart0_put_string(msg->mtext);
			uart0_put_string("\n\r");
		}
		set_process_priority(4,MEDIUM);
		release_processor();
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
