#include "pin.H"
#include "MyPinToolUtils.h"
#include <pthread.h>

extern std::ostream *out;
extern PIN_LOCK lock;
extern UINT64 Low;
extern UINT64 High;
extern UINT64 Start_addr;

//extern std::map<THREADID tid,std::string>function_map;
std::map<ADDRINT, pin_tracker*>race_map;
extern event_tracker CS[100];
extern race_issues* list;
extern race_issues** list_head;
/*
Check Thread Id if same mark as safe.
Case R -> R  -> Safe?
Case R -> W  -> Unsafe?
Case W -> R  -> Unsafe?
*/

VOID BeforeThreadCreate(ADDRINT* lock_nam, void* strt_rtn ,THREADID threadid)
{
    // cout << "strt_rtn " << (ADDRINT*)strt_rtn <<std::endl;
    // cout << "before pthread create... setting event flag: " << *lock_nam << " " << threadid << endl;
    CS[threadid].in_event = EVENT;
    PIN_ReleaseLock(&lock);
}

VOID AfterThreadCreate(THREADID threadid)
{
    PIN_GetLock(&lock, threadid+1);
    // cout << "after create passed parameters: " << threadid << endl;
    PIN_ReleaseLock(&lock);
}

VOID BeforeThreadJoin(ADDRINT lock_nam,THREADID threadid)
{
    PIN_GetLock(&lock, threadid+1);
    // cout << "before join passed parameters: " << lock_nam << " " << threadid << endl;
    PIN_ReleaseLock(&lock);
}

VOID AfterThreadJoin(THREADID threadid)
{
    PIN_GetLock(&lock, threadid+1);
    // cout << "after thread join clearing the event flag: " << threadid << endl;
    CS[threadid].in_event = NO_EVENT;
    PIN_ReleaseLock(&lock);
}

// This routine is executed each time sem wait is called.
VOID BeforeSemWait( ADDRINT size, THREADID threadid )
{
    PIN_GetLock(&lock, threadid+1);
    // *out << "thread " << threadid << "entered sem[" << "]" << endl;
    PIN_ReleaseLock(&lock);
}

// This routine is executed each time sem post is called.
VOID BeforeSemPost( ADDRINT size, THREADID threadid )
{
    PIN_GetLock(&lock, threadid+1);
    // *out << "thread " << threadid << "entered sem post [" << "]" << endl;
    PIN_ReleaseLock(&lock);
}

// This routine is executed each time mutex lock is called.
VOID AfterMutexLock(THREADID threadid )
{
    PIN_GetLock(&lock, threadid+1);
    // *out << "thread [" <<threadid << "] " << "In CS" << endl;
    CS[threadid].in_cs = SAFE;
    PIN_ReleaseLock(&lock);
}

// This routine is executed each time mutex lock is called.
VOID AfterMutexUnlock(THREADID threadid )
{
    PIN_GetLock(&lock, threadid+1);
    // *out << "thread [" <<threadid << "] " << "left CS" << endl;
    CS[threadid].in_cs = UNSAFE;
    PIN_ReleaseLock(&lock);
}

// This routine is executed each time mutex lock is called.
VOID BeforeMutexLock(ADDRINT* lock_name, THREADID threadid )
{
    PIN_GetLock(&lock, threadid+1);
    // *out << "thread [" << threadid << "] " << "requesting enter CS" << "[" << (void*)*lock_name << "]" << endl;
    PIN_ReleaseLock(&lock);
}

// This routine is executed each time mutex unlock is called.
VOID BeforeMutexUnlock(ADDRINT* lock_name, THREADID threadid )
{
    PIN_GetLock(&lock, threadid+1);
    // *out << "thread [" << threadid << "] " << "request exiting CS" << "[" << (void*)*lock_name << "]" << endl;
    PIN_ReleaseLock(&lock);
}

// Print a memory read record
VOID RecordMemRead(VOID * ip, VOID * addr, THREADID threadid )
{
    // fprintf(trace,"%p: R %p\n", ip, addr);
    // pin_tracker* read_track;

    PIN_GetLock(&lock, threadid+1);
    if((ADDRINT)ip < High && (ADDRINT)addr > Start_addr)
    {
        read_map((ADDRINT)addr, threadid,READ);
        // *out << "thread ["<< threadid <<"] " <<"R " << addr << " ip: " << ip << endl;
    }
    PIN_ReleaseLock(&lock);
}

// Print a memory write record
VOID RecordMemWrite(VOID * ip, VOID * addr, THREADID threadid )
{
    PIN_GetLock(&lock, threadid+1);
    if((ADDRINT)ip < High && (ADDRINT)addr > Start_addr)
    {
        // *out << "thread ["<< threadid <<"] "<< "W " << addr  <<  " ip: " << ip  << endl;
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
        // *out << "READ after READ" << endl;
        return SAFE;
    }
    // we have a write after read  condition possibly unsafe!
    else if(mem_region->read == READ && read == WRITE){
        // *out << "WRITE after READ " << endl;
        return SAFE;
    }

    // we have a write after write case
    else if(mem_region->read == WRITE && read == WRITE){
        // *out << "WRITE after WRITE " << endl;
        return UNSAFE;
    }

    // we have a read after write case
    else if(mem_region->read == WRITE && read == READ){
        // *out << "READ after WRITE " << endl;
         return UNSAFE;
    }
    return 0;
}


VOID read_map(ADDRINT addr, THREADID threadid, bool read)
{

    std::map<ADDRINT,pin_tracker*>::const_iterator got = race_map.find (addr);
    pin_tracker* write_track = NULL;
    int safe=UNSAFE;

    /*
    * check if we have seen this address before if we have not create a new entry
    */ 
    if ( got == race_map.end() )
    {
        // *out << "address not found: " << addr << endl;
        write_track = new pin_tracker;
        write_track->threadid = threadid;
        write_track->read = false;
        write_track->shared_mem = false; 
        race_map[(ADDRINT)addr] = write_track;

    }
    /*
    * we have seen the address so lets analyze
    */
    else
    {
        /*
        * log the event
        */
        //*out << "thread [" << got->second->threadid << "] ";
        // if(got->second->read)
        //     *out << "is reading: " << got->first << endl;
        // else
        //     *out << "is writing: " << got->first << endl;

        // if we have shared memory we need to check for a data race
        if(got->second->shared_mem == true)
        {
            safe = analyze_map(got->second,read);
        }

        // if this is the first instance of a different thread accessing the memory set to shared and check for a race
        else if(got->second->threadid != threadid)
        {
            //printf("id else  [%d], [%d]\n",threadid, got->second->threadid);
            got->second->shared_mem = true;
            safe = analyze_map(got->second,read);
        }
        else
            safe = SAFE; 

        // if the last thread that modified created a sync event check it
        // ie fork join 
        // the list contains all mem addresses written to at the time
        // if we are writing to a variable in an event setup write to the event list
        if( CS[threadid].in_event == EVENT )
        {
            add_to_event(&CS[threadid].event_addrs,addr);
            // std::cout << "adding to event " << std::endl;
        }
        // std::cout << threadid << " thread id: " << got->second->threadid << " " << addr << std::endl;

        // check if last thread created an event
        // if there is an event this list contains only addresses written to
        // AFTER the event ie AFTER a FORK or pthread_create
        if( CS[got->second->threadid].in_event == EVENT )
        {
            // std::cout << "checking event quue " << std::endl;
            // if list contains the address you are lookng for its unsafe
            if(contains_event(&CS[got->second->threadid].event_addrs,addr))
            {
                // std::cout << "contains event address " << std::endl;
                safe = UNSAFE;
            }
            else{
                // std::cout << "does not contain " <<std::endl;
                safe = SAFE;
            }
        }
        //if the thread doe snot have a lock acquired and we deem this entry as unsafe add it to our issue list
        if(CS[threadid].in_cs == UNSAFE && safe == UNSAFE && got->second->threadid != threadid)
        {

            // std::cout << "adding to race list " << addr << " " << threadid << " " << got->second->threadid<< std::endl;
            if(!contains(list_head,(ADDRINT*)got->first))
                add_to_effected(list_head,(void*)addr,got->second->threadid, threadid);
            //print_issue_queue(list_head);
            got->second->threadid = threadid;
        }

        // set read or write
        got->second->read = read;
    }


}

/*
* deallocates all the pointers allocated on the heap
*/
VOID clean_map()
{

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