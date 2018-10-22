#include "pin.H"
#include "MyPinTool.h"
#include <pthread.h>

extern std::ostream *out;
extern PIN_LOCK lock;
extern UINT64 Low;
extern UINT64 High;
extern UINT64 Start_addr;

std::map<ADDRINT, pin_tracker*>race_map;

// extern RTN_COUNT * RtnList; // LL 

// This routine is executed each time sem wait is called.
VOID BeforeSemWait( ADDRINT size, THREADID threadid )
{
    PIN_GetLock(&lock, threadid+1);
    *out << "thread " << threadid << "entered sem[" << "]" << endl;
    PIN_ReleaseLock(&lock);
}

// This routine is executed each time sem post is called.
VOID BeforeSemPost( ADDRINT size, THREADID threadid )
{
    PIN_GetLock(&lock, threadid+1);
    *out << "thread " << threadid << "entered sem post [" << "]" << endl;
    PIN_ReleaseLock(&lock);
}

// This routine is executed each time mutex lock is called.
VOID AfterMutexLock(char* name, THREADID threadid )
{
    PIN_GetLock(&lock, threadid+1);
    *out << "thread [" <<threadid << "] " << name << endl;
    PIN_ReleaseLock(&lock);
}

// This routine is executed each time mutex lock is called.
VOID AfterMutexUnlock(char* name, THREADID threadid )
{
    PIN_GetLock(&lock, threadid+1);
    *out << "thread [" <<threadid << "] " << name << endl;
    PIN_ReleaseLock(&lock);
}

// This routine is executed each time mutex lock is called.
VOID BeforeMutexLock(char* name, ADDRINT* lock_name, THREADID threadid )
{
    PIN_GetLock(&lock, threadid+1);
    *out << "thread [" << threadid << "] " << name << "[" << (pthread_mutex_t*)lock_name << "]" << endl;
    PIN_ReleaseLock(&lock);
}

// This routine is executed each time mutex unlock is called.
VOID BeforeMutexUnlock(char* name,ADDRINT* lock_name, THREADID threadid )
{
    PIN_GetLock(&lock, threadid+1);
    *out << "thread [" << threadid << "] " << name << "[" << (pthread_mutex_t*)lock_name << "]" << endl;
    PIN_ReleaseLock(&lock);
}

// Print a memory read record
VOID RecordMemRead(REG reg, VOID * ip, VOID * addr, THREADID threadid )
{
    // fprintf(trace,"%p: R %p\n", ip, addr);
    // pin_tracker* read_track;

    PIN_GetLock(&lock, threadid+1);
    if((ADDRINT)ip < High && (ADDRINT)addr > Start_addr)
    {
        read_map((ADDRINT)addr, threadid,WRITE);
        *out << "thread ["<< threadid <<"] " <<"R " << (ADDRINT)addr << " ip: " << ip << endl;
    }
    PIN_ReleaseLock(&lock);
}

// Print a memory write record
VOID RecordMemWrite(VOID * ip, VOID * addr, THREADID threadid )
{
    PIN_GetLock(&lock, threadid+1);
    if((ADDRINT)ip < High && (ADDRINT)addr > Start_addr)
    {
        *out << "thread ["<< threadid <<"] "<< "W " << (ADDRINT)addr  <<  " ip: " << ip  << endl;
        read_map((ADDRINT)addr, threadid,READ);
    }
    PIN_ReleaseLock(&lock);
}


VOID read_map(ADDRINT addr, THREADID threadid, bool read)
{

    std::map<ADDRINT,pin_tracker*>::const_iterator got = race_map.find (addr);
    pin_tracker* write_track = NULL;

    if ( got == race_map.end() ){
        *out << "address not found: " << addr << endl;
        write_track = new pin_tracker;
        write_track->threadid = threadid;
        write_track->read = false;
        race_map[(ADDRINT)addr] = write_track;
    }
    else{
        *out << "thread [" << got->second->threadid << "] ";
        if(got->second->read)
            *out << "is reading: " << got->first << endl;
        else
            *out << "is writing: " << got->first << endl;

        got->second->threadid = threadid;
        got->second->read = read;

    }


}

// store_map(ADDRINT ip, THREADID tid)
// {

// }

// char ll_conains()

// branch prediction stuff
// /*
//  * Analysis routines
//  */
// VOID Taken( const CONTEXT * ctxt)
// {
//     ADDRINT TakenIP = (ADDRINT)PIN_GetContextReg( ctxt, REG_INST_PTR );
//     OutFile << "Taken: IP = " << hex << TakenIP << dec << endl;
// }

// VOID Before(CONTEXT * ctxt)
// {
//     ADDRINT BeforeIP = (ADDRINT)PIN_GetContextReg( ctxt, REG_INST_PTR);
//     OutFile << "Before: IP = " << hex << BeforeIP << dec << endl;
// }


// VOID After(CONTEXT * ctxt)
// {
//     ADDRINT AfterIP = (ADDRINT)PIN_GetContextReg( ctxt, REG_INST_PTR);
//     OutFile << "After: IP = " << hex << AfterIP << dec << endl;
// }

    
// /*
//  * Instrumentation routines
//  */
// VOID ImageLoad(IMG img, VOID *v)
// {
//     for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
//     {
//         // RTN_InsertCall() and INS_InsertCall() are executed in order of
//         // appearance.  In the code sequence below, the IPOINT_AFTER is
//         // executed before the IPOINT_BEFORE.
//         for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
//         {
//             // Open the RTN.
//             RTN_Open( rtn );
            
//             // IPOINT_AFTER is implemented by instrumenting each return
//             // instruction in a routine.  Pin tries to find all return
//             // instructions, but success is not guaranteed.
//             RTN_InsertCall( rtn, IPOINT_AFTER, (AFUNPTR)After,
//                             IARG_CONTEXT, IARG_END);
            
//             // Examine each instruction in the routine.
//             for( INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins) )
//             {
//                 if( INS_IsRet(ins) )
//                 {
//                     // instrument each return instruction.
//                     // IPOINT_TAKEN_BRANCH always occurs last.
//                     INS_InsertCall( ins, IPOINT_BEFORE, (AFUNPTR)Before,
//                                    IARG_CONTEXT, IARG_END);
//                     INS_InsertCall( ins, IPOINT_TAKEN_BRANCH, (AFUNPTR)Taken,
//                                    IARG_CONTEXT, IARG_END);
//                 }
//             }
//             // Close the RTN.
//             RTN_Close( rtn );
//         }
//     }
// }