#ifndef PIN_UTILS_H
#define PIN_UTILS_H
#include "pin.H"
#include <stdbool.h>
#include <stdint.h>

// This routine is executed each time malloc is called.
VOID BeforeSemWait( ADDRINT size, THREADID threadid );

// This routine is executed each time malloc is called.
VOID BeforeSemPost( ADDRINT size, THREADID threadid );

// This routine is executed each nametime malloc is called.
VOID BeforeMutexLock( char* name, ADDRINT* size, THREADID threadid );

// This routine is executed each time malloc is called.
VOID BeforeMutexUnlock( char* name,ADDRINT* size, THREADID threadid );

VOID RecordMemRead(REG reg,VOID * ip, VOID * addr,ADDRINT read, THREADID threadid );

VOID RecordMemWrite(VOID * ip, VOID * addr,ADDRINT write, THREADID threadid );

VOID AfterMutexLock(char* name, THREADID threadid );

VOID AfterMutexUnlock(char* name, THREADID threadid );

// a struct for a hash table each thread can see who has last been in region
typedef struct pin_tracker{

	THREADID threadid; // who has last seen
	bool read; // last action of read or write 

}pin_tracker;



#endif