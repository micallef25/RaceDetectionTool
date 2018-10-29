#ifndef LIST_H
#define LIST_H

#include "pin.H"
#include <stdlib.h>
#include <stdint.h>

// a struct that holds the meory address of the effected region.
// possibly add a LL for holding multiple instances of time.
typedef struct race_issues{

	THREADID threadid_1;
	THREADID threadid_2;
	ADDRINT* effected_addr;
	uint32_t time;
	struct race_issues* next;


}race_issues;


void print_issue_queue(race_issues** head);
int contains(race_issues** head, ADDRINT* mem_addr);
int remove_from_job_queue(race_issues** head, ADDRINT* mem_addr);
int add_to_effected(race_issues** head,void* mem_addr, THREADID tid1, THREADID tid2);


#endif