<<<<<<< HEAD
#include "k_rtx.h"
#include "timer.h"

void wc_process() {
	msgbuf* keyreg_env;
	keyreg_env = (msgbug*)request_memory_block();
	keyreg_env->mtype = KCD_REG;
	keyreg_env->mtext[0] = '%';
	keyreg_env->mtext[1] = 'W';
	keyreg env->mtext[2] = 'S';
	keyreg_env->mtext[3] = '\0';
	send_message(KCD_PROC_ID, (void*)keyreg_env);

	keyreg_env = (msgbug*)request_memory_block();
	keyreg_env->mtype = KCD_REG;
	keyreg_env->mtext[0] = '%';
	keyreg_env->mtext[1] = 'W';
	keyreg env->mtext[2] = 'T';
	keyreg_env->mtext[3] = '\0';
	send_message(KCD_PROC_ID, (void*)keyreg_env);

	keyreg_env = (msgbug*)request_memory_block();
	keyreg_env->mtype = KCD_REG;
	keyreg_env->mtext[0] = '%';
	keyreg_env->mtext[1] = 'W';
	keyreg env->mtext[2] = 'R';
	keyreg_env->mtext[3] = '\0';
	send_message(KCD_PROC_ID, (void*)keyreg_env);	
	
	int status = 0; // 0 means terminated; 1 means running;
	int time = 0;
	msgbuf* next_second_message;
	next_second_message = (msgbuf*)request_memory_block();
	next_second_message->mtype = NOTIFY_WALL_CLOCK;
	delayed_send(WALL_CLOCK_PROC_ID, (void*)next_second_message, 1000);

	while (1) {
		msgbuf* message = receive_message(NULL);
		if (message->mtype == NOTIFY_WALL_CLOCK) {
			
			msgbuf* next_second_message;
			next_second_message = (msgbuf*)request_memory_block();
			next_second_message->mtype = NOTIFY_WALL_CLOCK;
			delayed_send(WALL_CLOCK_PROC_ID, (void*)next_second_message, 1000);
			
			if (state) {
				release_memory_block((void*)message);
				msgbuf* message_to_crt;
				message_to_crt = (msgbuf*)request_memory_block();
				message_to_crt->mtype = CRT_REQ;
				int crt_time = time + current_time;
				int seconds = crt_time / 1000 % 60;
				int minutes = crt_time / 1000 / 60 % 60;
				int hours   = crt_time / 1000 / 60 / 60 % 24;
				int s1 = seconds / 10;
				int s0 = seconds % 10;
				int m1 = minutes / 10;
				int m0 = minutes % 10;
				int h1 = hours   / 10;
				int h0 = hours   % 10;
				*(message_to_crt->mtext)++ = h1 + '0';
				*(message_to_crt->mtext)++ = h0 + '0';
				*(message_to_crt->mtext)++ = ':';
				*(message_to_crt->mtext)++ = m1 + '0';
				*(message_to_crt->mtext)++ = m0 + '0';
				*(message_to_crt->mtext)++ = ':';
				*(message_to_crt->mtext)++ = s1 + '0';
				*(message_to_crt->mtext)++ = s0 + '0';
				*(message_to_crt->mtext)++ = '\n';
				*(message_to_crt->mtext)++ = '\r';
				*(message_to_crt->mtext)++ = '\0';
				send_message(CRT_PROC_ID, (void*)message_to_crt);
			} else {
				release_memory_block(message);
			}
		}
		
		if (message->mtype == KCD_REG) {
			switch (message->mtext[2]){
			case 'R': {
				state = 1;
				release_memory_block((void*)message);
				break;
			}
			
			case 'S': {
				int time_set;
				char* buf = &(message->mtext)[4];
				int h1 = *buf - '0';
				buf = &(message->mtext)[5];
				int h0 = *buf - '0';
				buf = &(message->mtext)[7];
				int m1 = *buf - '0';
				buf = &(message->mtext)[8];
				int m0 = *buf - '0';
				buf = &(message->mtext)[10];
				int s1 = *buf - '0';
				buf = &(message->mtext)[11];
				int s0 = *buf - '0';
				buf = &(message->mtext)[12];
				int ter = *buf;
				if(h1 > 2 || h1 < 0 || h0 > 9 || h0 < 0 || (h1 == 2 && h0 > 3) || m1 > 6 || m1 < 0 || m0 > 9 || m0 < 0 || s1 > 6 || s1 < 0 || s0 > 9 || s0 < 0 || ter != '\0'){
					time_set = -1;
				}
				else{
					time_set = 1000*((h1*10 + h0)*360 + (m1*10 + m0)*60 + (s1*10 + s0));	
				}	
				if(time_set < 0){
					state = 0;
					message->mtype = CRT_REQ;
					message->mtest[0] = 'E';
					message->mtest[1] = 'R';
					message->mtest[2] = 'R';
					message->mtest[3] = '\n';
					message->mtest[4] = '\r';
					send_message(CRT_PROC_ID, (void*)message);
				}
				else{
				  state = 1;
				  time_base = time_set - current_time;
				  release_memory_block(message);
				}
				break;
		  }
			
		  case 'T': {
				state = 0;
				release_memory_block(message);
				break;			
			}

			default: {
					// should print some message here
					state = 0;
					release_memory_block(message);
					break;
			}				
			
			}
		}
	}
}
=======
/*
#include "rtx.h"
#include "timer.h"
#include "stdint.h"

static void print_time(char* buf, int time) {
	int seconds, minutes, hours, s0, s1, m0, m1, h0, h1; 
	seconds = time / 1000 % 60;
	minutes = time / 1000 / 60 % 60;
	hours   = time / 1000 / 60 / 60 % 24;
	s0 = seconds % 10
  s1 = seconds / 10;
  m0 = minutes % 10;
	m1 = minutes / 10;
  h0 = hours   % 10;
	h1 = hours   / 10;

	*buf++ = h1 + '0';
	*buf++ = h0 + '0';
	*buf++ = ':';
	*buf++ = m1 + '0';
	*buf++ = m0 + '0';
	*buf++ = ':';
	*buf++ = s1 + '0';
	*buf++ = s0 + '0';
	*buf++ = '\n';
	*buf++ = '\r';
	*buf++ = '\0';
}

static int parse_time(char* message_buffer) {
	// return absolute time; 
	char* buf = &message_buffer[4]; // %WS HH:MM:SS; validity check here;
	int h1 = *buf++ - '0';
	int h0 = *buf++ - '0';
	int colon0 = *buf++;
	int m1 = *buf++ - '0';
	int m0 = *buf++ - '0';
	int colon1 = *buf++;
	int s1 = *buf++ - '0';
	int s0 = *buf++ - '0';
	int terminate = *buf++;
	// negative value should never appear 
	if (h1 > 2 || h1 < 0 || h0 > 9 || h0 < 0) {
		//strcpy(message_buffer, "Invalid hour format!\n\r");
		return -1;
	}
	if (h1 == 2 && h0 > 3) {
		//strcpy(message_buffer, "Invalid hour format\n\r");
		return -1;
	}
	if (colon0 != ':') {
		//strcpy(message_buffer, "Missing colon\n\r");
		return -1;
	}
	if (m1 > 6 || m1 < 0 || m0 > 9 || m0 < 0) {
		//strcpy(message_buffer, "Invalid minute format!\n\r");
		return -1;
	}
	if (colon1 != ':') {
		//strcpy(message_buffer, "Missing colon\n\r");
		return -1;
	}
	if (s1 > 6 || s1 < 0 || s0 > 9 || s0 < 0) {
		//strcpy(message_buffer, "Invalid second format!\n\r");
		return -1;
	}
	if (terminate != '\0') {
		//strcpy(message_buffer, "Missing null terminator!\n\r");
		return -1;
	}

	return 1000 * (
		  h1 * 60 * 60 * 10
		+ h0 * 60 * 60
		+ m1 * 60 * 10
		+ m0 * 60
		+ s1 * 10
		+ s0);
	
}

void wall_clock_process() {
	char RESET_CMD, SET_CMD, TERMINATE_CMD;
	int current_time, current_state, index, message_length;
	char* message_trans;
	RESET_CMD = 'R';
	SET_CMD = 'S';
	TERMINATE_CMD = 'T';

	// All times are stored in milliseconds, state = 0 means not_running 
	current_time = 0;
	current_state = 0; 
	index = 0;

	struct msgbuf* key_reg_envelope = (struct msgbuf*)request_memory_block();
	key_reg_envelope->mtype = KCD_REG;
	key_reg_envelope->mtext[0] = 'W';
	key_reg_envelope->mtext[1] = '\0';
	send_message(PROCESS_ID_KCD, (void*)key_reg_envelope);

	struct msgbuf* timer_message = (struct msgbuf*)request_memory_block();
		// change timer_message->mtype to our def, need a wall clock id; 
			timer_message->mtype = MESSAGE_TYPE_WALL_CLOCK;
		delayed_send(PROCESS_ID_WALL_CLOCK, (void*)timer_message, 1000);

	while (1) {
		struct msgbuf* message = receive_message(NULL);
		if (message == NULL) {
			// should never reach here
		}
		
		if (message->mtext[2] == RESET_CMD) {
			current_state = 1;
		    // I need a timer func to know how much time elapsed. 
			  current_time = 0 - timer_elapsed_ms();
			release_memory_block(message);
		}
		else if (message->mtext[2] == SET_CMD) {
			int offset = parse_time(message->mtext);
			if (offset < 0) {
				current_state = 0;
			  	// change timer_message->mtype to our def
					message->mtype = MESSAGE_TYPE_CRT_DISPLAY_REQUEST;
				send_message(CRT_PROC_ID, (void*)message);
			}
			current_state = 1;
			  // I need a timer func to know how much time elapsed. 
				current_time = offset - timer_elapsed_ms();
			release_memory_block(message);
		}
		else if (message->mtext[2] == TERMINATE_CMD) {
			current_state = 0;
			release_memory_block(message);
		}
		else {
			// invalid cmds
			release_memory_block(message);
		} 
		
			struct msgbuf* timer_message = (struct msgbuf*)request_memory_block();
				// change timer_message->mtype to our def 
				timer_message->mtype = MESSAGE_TYPE_WALL_CLOCK;
			  delayed_send(PROCESS_ID_WALL_CLOCK, (void*)timer_message, 1000);

			if (current_state) {
				//get a clear message
				message_length = sizeof(*message);
				//message_trans = (char*)message;
				for (; index<message_length; index++){
					((char*)message)[index] = 0x00;
				}
				message->mtype = MESSAGE_TYPE_CRT_DISPLAY_REQUEST;
				print_time(message->mtext, timer_elapsed_ms() + time_base);
				send_message(CRT_PROC_ID, (void*)message);
			} else {
				release_memory_block(message);
			}
			break;
		}
	}
}
*/
>>>>>>> f91bc131d28d5656700204a2461b6350287f71fe
