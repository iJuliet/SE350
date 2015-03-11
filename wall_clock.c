/*

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
*/

