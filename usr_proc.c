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

/* initialization table item */
PROC_INIT g_test_procs[NUM_TEST_PROCS];
int test_num; // this indicates which test are we on
int ok_tests;

typedef struct _msgbuf {
	int mtype; /* user defined message type */
	char mtext[1]; /* body of the message */
} msgbuf;

void set_test_procs() {
	int i;
	test_num = 1;
	ok_tests = 0;
	for( i = 0; i < NUM_TEST_PROCS; i++ ) {
		g_test_procs[i].m_pid=(U32)(i+1);
		g_test_procs[i].m_priority=LOWEST;
		g_test_procs[i].m_stack_size=0x100;
	}
  
	g_test_procs[0].mpf_start_pc = &proc1;
	g_test_procs[1].mpf_start_pc = &proc2;
	g_test_procs[2].mpf_start_pc = &proc3;
	g_test_procs[3].mpf_start_pc = &proc4;
}

/**
 * @brief: a process that prints five uppercase letters
 *         and then yields the cpu.
 */
void proc1(void)
{
	int i, ret_val;
	i = 0;
	
	
	while (1) {
		if(test_num == 1){
				uart0_put_string("G013_test: START\n\r");
				uart0_put_string("G013_test: Total 8 tests\n\r");
				set_process_priority(3,MEDIUM);
				if(get_process_priority(3) == MEDIUM){
					uart0_put_string("G013_test: test 1 OK\n\r");	
					test_num ++;
					ok_tests ++;
				}else{
					uart0_put_string("G013_test: test 1 FAIL\n\r");	
					test_num ++;
				}
		}
		if ( i != 0 && i%5 == 0 ) {
			ret_val = release_processor();
			if(test_num == 2){
				uart0_put_string("G013_test: test 2 FAIL\n\r");
				test_num++;
			}
			
#ifdef DEBUG_0
			printf("proc1: ret_val=%d\n", ret_val);
#endif /* DEBUG_0 */
		}
		//uart0_put_char('A' + i%26);
		i++;
	}
}

/**
 * @brief: a process that prints five numbers
 *         and then yields the cpu.
 */
void proc2(void)
{
	
	int i = 0;
	int ret_val = 20;
	if(test_num == 8){
		uart0_put_string("G013_test: test 8 OK\n\r");
		ok_tests++;
		printEndTestString();
		test_num++;
	}
	while ( 1) {
		if ( i != 0 && i%5 == 0 ) {
			ret_val = release_processor();
		
#ifdef DEBUG_0
			printf("proc2: ret_val=%d\n", ret_val);
#endif /* DEBUG_0 */
		}
		i++;
	}
}

/**
 * @brief: a process that request a memory block
 *        
 */
void proc3(void)
{
	int status;
	U32* mem_addr;
	while (1) {
		if( test_num == 2){
			//first time we execute this process
			uart0_put_string("G013_test: test 2 OK\n\r");
			test_num++;
			ok_tests++;
		}
		if (test_num == 6){
			//second time we execute this process
			uart0_put_string("G013_test: test 6 OK\n\r");
			test_num++;
			ok_tests++;
			//set proc4 to the same priority
			set_process_priority(4,MEDIUM);
		}
		
		
		

#ifdef DEBUG_0
	printf("proc3: mem_addr=%d\n", mem_addr);
#endif /*DEBUG_0*/
		if(test_num == 3){
			mem_addr = (U32 *)request_memory_block();
			if(mem_addr != NULL){
					uart0_put_string("G013_test: test 3 OK\n\r");
					ok_tests++;
					
			}else{
				uart0_put_string("G013_test: test 3 FAIL\n\r");
			}
			test_num++;
		}
		
		if(test_num == 4) {
			status = release_memory_block((U32 *)mem_addr);
			if(status == RTX_OK ){
				uart0_put_string("G013_test: test 4 OK\n\r");
				ok_tests++;
			}else{
				uart0_put_string("G013_test: test 4 FAIL\n\r");
			}
			test_num++;
		}
		
		if(test_num == 5) {
			//try release again, which should give us an error
			status = release_memory_block((U32*) mem_addr);
			if(status == RTX_ERR) {
				uart0_put_string("G013_test: test 5 OK\n\r");
				ok_tests++;
			}else{
				uart0_put_string("G013_test: test 5 FAIL\n\r");
			}
			test_num++;
		}
		
		release_processor();
	}
}


void proc4(void)
{
	U32 *mem_addr;
	int i, ret_val;

	if(test_num == 7){
		uart0_put_string("G013_test: test 7 OK\n\r");
		test_num++;
		ok_tests++;
	} else{
		uart0_put_string("G013_test: test 7 FAIL\n\r");
		test_num++;
	}
	set_process_priority(2,HIGH);
	//set_process_priority(4,HIGH);
	//request thousands of memory block which should fail in the end
	for(i=0 ; i<2000; ++i) {
		mem_addr = (U32 *)request_memory_block();
	}
	//should to go to proc2
	if(test_num == 8){
		uart0_put_string("G013_test: test 8 FAIL\n\r");
		printEndTestString();
		test_num++;
	}
	release_processor();

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
