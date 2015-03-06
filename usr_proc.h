/**
 * @file:   usr_proc.h
 * @brief:  Four user processes for tests and one proc for printing test result header file
 * @author: G013
 * @date:   2014/02/05
 */
 
#ifndef USR_PROC_H_
#define USR_PROC_H
#define RTX_ERR -1
#define RTX_OK  0

void set_test_procs(void);
void proc1(void);
void proc2(void);
void proc3(void);
void proc4(void);
void printEndTestString(void);
#endif /* USR_PROC_H_ */
