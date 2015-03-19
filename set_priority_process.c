#include "set_priority_process.h"
#include "uart_polling.h"
#include "string.h"


void set_priority_process(void) {
	
	MSGBUF* msg;
	msg = request_memory_block();
	msg->mtype = KCD_REG;
	msg->mtext[0] = '%';
	msg->mtext[1] = 'C';
	msg->mtext[2] = '\0';
	send_message(KCD_PROC_ID,msg);
	while(1){
		int pid, new_priority, index;
		int digit;
		msg = (MSGBUF*)receive_message(NULL);
		
		if (msg == NULL || msg->mtype != DEFAULT) {
				release_memory_block(msg);
		} else {
			index = 3;
			while(msg->mtext[index] != ' '){
				digit = msg->mtext[index] - '0';
				pid = pid*10 + digit;
				index++;
			}
			//uart0_put_char(pid+'0');
			while(msg->mtext[index] == ' ') {
				index++;
			}
			while(msg->mtext[index] != ' '){
				digit = msg->mtext[index] - '0';
				new_priority = new_priority*10 + digit;
				index++;
			}
			
			if ((pid >= 1 && pid <= 6) || pid == 11 ) {
				set_process_priority(pid, new_priority);
				release_memory_block(msg);
			} else {
				//display error message
				char text[] = "Permission denied: cannot set non-user process priority.\r\n";
				msg->mtype = CRT_REQ;
				strncpy(msg->mtext, text, strlen(text));
				send_message(CRT_PROC_ID, msg);
				
			}
		}
		release_processor();
	}
}
