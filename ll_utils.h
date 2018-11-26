#ifndef LIST_H
#define LIST_H

#include "pin.H"
#include <stdlib.h>
#include <stdint.h>
#include <iostream>

// a struct that holds the meory address of the effected region.
// possibly add a LL for holding multiple instances of time.
typedef struct race_issues{

	THREADID threadid_1;
	THREADID threadid_2;
	ADDRINT* effected_addr;
	uint32_t time;
	struct race_issues* next;

}race_issues;

typedef struct event_list{

	ADDRINT address;
	struct event_list* next;

}event_list;

// a struct for checking if we are in a CS or not or if a event has occured
// on writes we clear the event flag on special events we set the flag.
typedef struct event_tracker{

	uint8_t in_cs;
	uint8_t in_event;
	struct event_list* event_addrs;
	
}event_tracker;


void print_issue_queue(race_issues** head);
int contains(race_issues** head, ADDRINT* mem_addr);
int remove_from_job_queue(race_issues** head, ADDRINT* mem_addr);
int add_to_effected(race_issues** head,void* mem_addr, THREADID tid1, THREADID tid2);
void log_issue_queue(race_issues** head,std::ostream* fp);

int delete_event_queue (event_list** head);
void print_event_queue(event_list** head);
int add_to_event(event_list** head,ADDRINT mem_addr);
int contains_event(event_list** head, ADDRINT mem_addr);


#endif