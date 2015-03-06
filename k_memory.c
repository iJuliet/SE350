/**
 * @file:   k_memory.c
 * @brief:  kernel memory managment routines
 * @author: G013
 * @date:   2014/02/05
 */

#include "k_memory.h"
#include "k_process.h"
#include "uart_polling.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

/* ----- Global Variables ----- */
U32 *gp_stack; /* The last allocated stack low address. 8 bytes aligned */
               /* The first stack starts at the RAM high address */
	       /* stack grows down. Fully decremental stack */
U32 *gp_heap;

const U32 BLOCK_SIZE = 32; //32instructions

typedef struct _mem_blk{
		struct _mem_blk* next_blk_ptr;
		//U32 *block_addr; 
		//uint32_t block_address;
} mem_blk;

mem_blk* mem_start_ptr;
mem_blk* mem_end_ptr;
U32* heap_limit;
/**
 * @brief: Initialize RAM as follows:

0x10008000+---------------------------+ High Address
          |    Proc 1 STACK           |
          |---------------------------|
          |    Proc 2 STACK           |
          |---------------------------|<--- gp_stack
          |                           |
          |        HEAP               |
          |                           |
          |---------------------------|<--- gp_heap
          |        PCB 2              |
          |---------------------------|
          |        PCB 1              |
          |---------------------------|
          |        PCB pointers       |
          |---------------------------|<--- gp_pcbs
          |        Padding            |
          |---------------------------|
          |Image$$RW_IRAM1$$ZI$$Limit |
          |...........................|          
          |       RTX  Image          |
          |                           |
0x10000000+---------------------------+ Low Address

*/

void memory_init(void)
{
	U8 *p_end = (U8 *)&Image$$RW_IRAM1$$ZI$$Limit;
	int i;
	U32 *next, *end;
  
	/* 4 bytes padding */
	p_end += 4;

	/* allocate memory for pcb pointers   */
	gp_pcbs = (PCB **)p_end;
	p_end += (int) ((NUM_TEST_PROCS+1) * sizeof(PCB *));
  
	for ( i = 0; i < NUM_TEST_PROCS+1; i++ ) {
		gp_pcbs[i] = (PCB *)p_end;
		p_end += sizeof(PCB); 
	}
#ifdef DEBUG_0  
	printf("gp_pcbs[0] = 0x%x \n", gp_pcbs[0]);
	printf("gp_pcbs[1] = 0x%x \n", gp_pcbs[1]);
#endif
	
	/* prepare for alloc_stack() to allocate memory for stacks */
	
	gp_stack = (U32 *)RAM_END_ADDR;
	if ((U32)gp_stack & 0x04) { /* 8 bytes alignment */
		--gp_stack; 
	}
  
	/* allocate memory for heap, not implemented yet*/
	//init linked list with available memory block
	gp_heap = (U32 *)p_end;
	heap_limit = (U32 *)((RAM_END_ADDR + (U32) p_end)/2);
	mem_start_ptr = (mem_blk *) gp_heap;
	mem_start_ptr -> next_blk_ptr = NULL;
	mem_end_ptr = mem_start_ptr;
	
	next = gp_heap + BLOCK_SIZE;
	end = heap_limit;
	while(next < end) {
		mem_blk *temp = (mem_blk *)next;
		temp->next_blk_ptr = NULL;
		mem_end_ptr -> next_blk_ptr = temp;
		mem_end_ptr = temp;
		next += BLOCK_SIZE;
	}
}

/**
 * @brief: allocate stack for a process, align to 8 bytes boundary
 * @param: size, stack size in bytes
 * @return: The top of the stack (i.e. high address)
 * POST:  gp_stack is updated.
 */

U32 *alloc_stack(U32 size_b) 
{
	U32 *sp;
	sp = gp_stack; /* gp_stack is always 8 bytes aligned */
	
	/* update gp_stack */
	gp_stack = (U32 *)((U8 *)sp - size_b);
	
	/* 8 bytes alignement adjustment to exception stack frame */
	if ((U32)gp_stack & 0x04) {
		--gp_stack; 
	}
	return sp;
}

void *k_request_memory_block(void) {
	PCB* curr_proc;
#ifdef DEBUG_0 
	printf("k_request_memory_block: entering...\n");
#endif /* ! DEBUG_0 */
	mem_blk* free_mem_blk;
	while (mem_start_ptr == NULL) {
		curr_proc = get_current_proc();
		bq_enqueue(curr_proc);
		curr_proc->m_state = BLK_ON_MEM;
		k_release_processor();
	}
	free_mem_blk = mem_start_ptr;
	mem_start_ptr = mem_start_ptr->next_blk_ptr;
	return (void *) free_mem_blk;
}

int k_release_memory_block(void *p_mem_blk) {
	PCB* blk_proc;
	mem_blk* temp;
#ifdef DEBUG_0 
	printf("k_release_memory_block: releasing block @ 0x%x\n", p_mem_blk);
#endif /* ! DEBUG_0 */
	//memory block pointer is not valid
	
	if( (U32 *)p_mem_blk >= (U32 *)gp_heap && 
		(U32 *)p_mem_blk <= (U32 *)heap_limit && 
	((U32 *)p_mem_blk - (U32 *)gp_heap)%BLOCK_SIZE != 0){
		return RTX_ERR;
	}
	
	//check if the block is in free memeory
	temp = mem_start_ptr;
	while(temp != NULL){
		if((U32 *)p_mem_blk - (U32 *)temp == 0){
			return RTX_ERR;
		}
		temp = temp -> next_blk_ptr;
	}
	//put memory block into heap
	((mem_blk*) p_mem_blk) -> next_blk_ptr = mem_start_ptr;
	mem_start_ptr = p_mem_blk;
	
	//set p_mem_blk to NULL
	p_mem_blk = NULL;

	blk_proc = bq_dequeue();
	if (blk_proc != NULL) {
		blk_proc->m_state = RDY;
		rpq_enqueue(blk_proc);
	}
	return RTX_OK;
}

