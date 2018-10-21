#ifndef PIN_UTILS_H
#define PIN_UTILS_H
#include "pin.H"
#include <stdbool.h>
#include <stdint.h>

VOID BeforeSemWait( ADDRINT size, THREADID threadid );

// This routine is executed before each sempost call.
VOID BeforeSemPost( ADDRINT size, THREADID threadid );

/*This routine is executed before each mutex called.
 * @param[in]   name			string passed in by callback argument as name of (variable can be changed)
 * @param[in]   lock_ptr        Ptr to lock requested in function call
 * @param[in]   threadId        unique thread id assigned by pin
 */
VOID BeforeMutexLock( char* name, ADDRINT* lock_ptr, THREADID threadid );

/*This routine is executed before each mutex called.
 * @param[in]   name			string passed in by callback argument as name of (variable can be changed)
 * @param[in]   lock_ptr        Ptr to lock requested in function call
 * @param[in]   threadId        unique thread id assigned by pin
 */
VOID BeforeMutexUnlock( char* name,ADDRINT* lock_ptr, THREADID threadid );

/*This routine is executed before each mutex called.
 * @param[in]   reg 		register object to figure out where we are loading to
 * @param[in]   ip 			addres of instruction pointer
 * @param[in]   addr 		address of address reading from
 * @param[in]   threadId    unique thread id assigned by pin
 */
VOID RecordMemRead(REG reg,VOID * ip, VOID * addr, THREADID threadid );


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
VOID AfterMutexLock(char* name, THREADID threadid );

/*This routine is executed after each mutex unlck called.
 * @param[in]   name			string passed in by callback argument as name of (variable can be changed)
 * @param[in]   threadId        unique thread id assigned by pin
 */
VOID AfterMutexUnlock(char* name, THREADID threadid );

// a struct for a hash table each thread can see who has last been in region
typedef struct pin_tracker{

	THREADID threadid; // who has last seen
	bool read; // last action of read or write 

}pin_tracker;



#endif