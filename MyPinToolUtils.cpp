#include "pin.H"
#include "MyPinToolUtils.h"
#include <pthread.h>

extern std::ostream *out;
extern PIN_LOCK lock;
extern UINT64 Low;
extern UINT64 High;
extern UINT64 Start_addr;

std::map<ADDRINT, pin_tracker*>race_map;
extern uint8_t CS[100];
extern race_issues* list;
extern race_issues** list_head;
/*
Check Thread Id if same mark as safe.
Case R -> R  -> Safe?
Case R -> W  -> Unsafe?
Case W -> R  -> Unsafe?
*/

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
VOID AfterMutexLock(THREADID threadid )
{
    PIN_GetLock(&lock, threadid+1);
    *out << "thread [" <<threadid << "] " << "In CS" << endl;
    CS[threadid] = SAFE;
    PIN_ReleaseLock(&lock);
}

// This routine is executed each time mutex lock is called.
VOID AfterMutexUnlock(THREADID threadid )
{
    PIN_GetLock(&lock, threadid+1);
    *out << "thread [" <<threadid << "] " << "left CS" << endl;
    CS[threadid] = UNSAFE;
    PIN_ReleaseLock(&lock);
}

// This routine is executed each time mutex lock is called.
VOID BeforeMutexLock(ADDRINT* lock_name, THREADID threadid )
{
    PIN_GetLock(&lock, threadid+1);
    *out << "thread [" << threadid << "] " << "requesting enter CS" << "[" << (void*)*lock_name << "]" << endl;
    PIN_ReleaseLock(&lock);
}

// This routine is executed each time mutex unlock is called.
VOID BeforeMutexUnlock(ADDRINT* lock_name, THREADID threadid )
{
    PIN_GetLock(&lock, threadid+1);
    *out << "thread [" << threadid << "] " << "request exiting CS" << "[" << (void*)*lock_name << "]" << endl;
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
        read_map((ADDRINT)addr, threadid,READ);
        *out << "thread ["<< threadid <<"] " <<"R " << addr << " ip: " << ip << endl;
    }
    PIN_ReleaseLock(&lock);
}

// Print a memory write record
VOID RecordMemWrite(VOID * ip, VOID * addr, THREADID threadid )
{
    PIN_GetLock(&lock, threadid+1);
    if((ADDRINT)ip < High && (ADDRINT)addr > Start_addr)
    {
        *out << "thread ["<< threadid <<"] "<< "W " << addr  <<  " ip: " << ip  << endl;
        read_map((ADDRINT)addr, threadid,WRITE);
    }
    PIN_ReleaseLock(&lock);
}

// this function will analyze whether or not the memory region is safe
// read = false for a write true for a read
int analyze_map(pin_tracker* mem_region, bool read)
{
    // *out << "record " << (mem_region->read ? " Read" : "Write") <<" new " << (read ? " Read" : "Write") <<  endl;

    // if this a read after read it is safe... for now... 
    if(mem_region->read == READ && read == READ){
        *out << "READ after READ" << endl;
        return SAFE;
    }
    // we have a write after read  condition possibly unsafe!
    else if(mem_region->read == READ && read == WRITE){
        *out << "WRITE after READ " << endl;
        return SAFE;
    }

    // we have a write after write case
    else if(mem_region->read == WRITE && read == WRITE){
        *out << "WRITE after WRITE " << endl;
        return UNSAFE;
    }

    // we have a read after write case
    else if(mem_region->read == WRITE && read == READ){
        *out << "READ after WRITE " << endl;
         return UNSAFE;
    }
    return 0;
}


VOID read_map(ADDRINT addr, THREADID threadid, bool read)
{

    std::map<ADDRINT,pin_tracker*>::const_iterator got = race_map.find (addr);
    pin_tracker* write_track = NULL;
    int safe=UNSAFE;

    if ( got == race_map.end() )
    {
        *out << "address not found: " << addr << endl;
        write_track = new pin_tracker;
        write_track->threadid = threadid;
        write_track->read = false;
        write_track->shared_mem = false; 
        race_map[(ADDRINT)addr] = write_track;
    }
    else
    {
        *out << "thread [" << got->second->threadid << "] ";
        if(got->second->read)
            *out << "is reading: " << got->first << endl;
        else
            *out << "is writing: " << got->first << endl;

        // *out << "shared access " << (got->second->shared_mem ? " Yes" : "No") << endl;
        // if we have shared memory we need to check for a data race
        if(got->second->shared_mem == true)
        {
            safe = analyze_map(got->second,read);
        }

        // if this is the first instance of a different thread accessing the memory set to shared and check for a race
        else if(got->second->threadid != threadid)
        {
            printf("id else  [%d], [%d]\n",threadid, got->second->threadid);
            got->second->shared_mem = true;
            safe = analyze_map(got->second,read);
            std::cout << "SHARED " << std::endl;
            std::cout << "safe: " << safe <<std::endl;
            printf("shared\n");
        }
        else
            safe = SAFE; 

        if(CS[threadid] == UNSAFE && safe == UNSAFE)
        {
            std::cout << "RACE CONDITION DETECTED ERROR ERROR WTF!!!!!"  << endl;
            printf("id unsafe [%d], [%d]\n",threadid, got->second->threadid);
            if(!contains(list_head,(ADDRINT*)got->first))
                add_to_effected(list_head,(void*)addr,got->second->threadid, threadid);
            print_issue_queue(list_head);
            got->second->threadid = threadid;
        }

        //got->second->threadid = threadid;
        got->second->read = read;
    }


}

VOID clean_map()
{
      // show content:
  for (std::map<ADDRINT,pin_tracker*>::iterator it=race_map.begin(); it!=race_map.end(); ++it)
    delete it->second;

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