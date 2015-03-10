#include "k_process.h"


void* k_receive_message(int* sender_id);
int k_send_message(int process_id, msgbuf *message_envelope);
