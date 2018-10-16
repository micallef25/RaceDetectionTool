#ifndef PIN_UTILS_H
#define PIN_UTILS_H
#include "pin.H"

// This routine is executed each time malloc is called.
VOID BeforeSemWait( ADDRINT size, THREADID threadid );

// This routine is executed each time malloc is called.
VOID BeforeSemPost( ADDRINT size, THREADID threadid );

// This routine is executed each time malloc is called.
VOID BeforeMutexLock( ADDRINT* size, THREADID threadid );

// This routine is executed each time malloc is called.
VOID BeforeMutexUnlock( ADDRINT* size, THREADID threadid );


#endif