#include "k_process.h"

#ifndef MESSAGE_H_
#define MESSAGE_H_


void* k_receive_message(int*);
int k_send_message(int, void *);
int k_send_message_no_preemp(int, void *);
int k_delayed_send(int, void*, int);
void* msg_dequeue(PCB*, int*);

#endif
