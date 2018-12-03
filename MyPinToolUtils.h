#ifndef PIN_UTILS_H
#define PIN_UTILS_H
#include "pin.H"
#include "ll_utils.h"
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <map>

#define READ 1
#define WRITE 0
#define SAFE 1
#define UNSAFE 0 
#define EVENT 1
#define NO_EVENT 0

VOID BeforeSemWait( ADDRINT size, THREADID threadid );

// This routine is executed before each sempost call.
VOID BeforeSemPost( ADDRINT size, THREADID threadid );

/*This routine is executed before each mutex called.
 * @param[in]   name			string passed in by callback argument as name of (variable can be changed)
 * @param[in]   lock_ptr        Ptr to lock requested in function call
 * @param[in]   threadId        unique thread id assigned by pin
 */
VOID BeforeMutexLock(ADDRINT* lock_ptr, THREADID threadid );

/*This routine is executed before each mutex called.
 * @param[in]   name			string passed in by callback argument as name of (variable can be changed)
 * @param[in]   lock_ptr        Ptr to lock requested in function call
 * @param[in]   threadId        unique thread id assigned by pin
 */
VOID BeforeMutexUnlock(ADDRINT* lock_ptr, THREADID threadid );

/*This routine is executed before each mutex called.
 * @param[in]   reg 		register object to figure out where we are loading to
 * @param[in]   ip 			addres of instruction pointer
 * @param[in]   addr 		address of address reading from
 * @param[in]   threadId    unique thread id assigned by pin
 */
VOID RecordMemRead(VOID * ip, VOID * addr, THREADID threadid );


/*This routine is executed before each mutex called.
 * @param[in]   reg 		register object to figure out where we are loading to
 * @param[in]   ip 			addres of instruction pointer
 * @param[in]   addr 		address of address written to
 * @param[in]   threadId    unique thread id assigned by pin
 */
VOID RecordMemWrite(VOID * ip, VOID * addr, THREADID threadid );

/*This routine is executed after each mutex lock is called.
 * @param[in]   name			string passed in by callback argument as name of (variable can be changed)
 * @param[in]   threadId        unique thread id assigned by pin
 */
VOID AfterMutexLock(THREADID threadid );

/*This routine is executed after each mutex unlck called.
 * @param[in]   name			string passed in by callback argument as name of (variable can be changed)
 * @param[in]   threadId        unique thread id assigned by pin
 */
VOID AfterMutexUnlock(THREADID threadid );


VOID AfterThreadCreate(THREADID threadid);

VOID AfterThreadJoin(THREADID threadid);

VOID BeforeThreadCreate(ADDRINT* lock_nam  ,void* strt_rtn,THREADID threadid);

VOID BeforeThreadJoin(ADDRINT lock_nam,THREADID threadid);

VOID read_map(ADDRINT addr, THREADID threadid, bool read);

void clean_map();

// a struct for a hash table each thread can see who has last been in region
typedef struct pin_tracker{

	THREADID threadid; // who has last seen
	bool read; // last action of read or write
	bool shared_mem; // false for local true for shared amongst threads

}pin_tracker;


#endif