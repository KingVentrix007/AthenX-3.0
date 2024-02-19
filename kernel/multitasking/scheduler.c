//===================================================================================================================
//
//  scheduler.cc -- These are the scheduler functions
//
// -----------------------------------------------------------------------------------------------------------------
//
//     Date      Tracker  Version  Pgmr  Description
//  -----------  -------  -------  ----  --------------------------------------------------------------------------
//  2019-Sep-22  Initial  step01   ADCL  Initial version
//  2019-Sep-25  Step 2   step02   ADCL  Add `Schedule()` and create a circular linked list
//  2019-Oct-05  Step 3   step03   ADCL  Add timer-related functions to the scheduler
//  2019-Oct-05  Step 4   step04   ADCL  Updates for process states
//  2019-Oct-24  Step 5   step05   ADCL  Add rudamentary scheduler lock
//  2019-Oct-25  Step 6   step06   ADCL  Add the ability to block/unblock
//  2019-Nov-01  Step 8   step08   ADCL  Add checks for postponed task changes
//  2019-Nov-05  Step 9   step09   ADCL  Add sleeping to the process repetoire
//  2019-Nov-09  Step10   step10   ADCL  Add idle CPU handling 
//  2019-Nov-10  Step11   step11   ADCL  Add preemption
//  2019-Nov-20  Step12   step12   ADCL  Add process termination
//
//===================================================================================================================


#include "cpu.h"
#include "scheduler.h"
#include "types.h"
#include "stdbool.h"
#include "fat_filelib.h"
//
// -- PUSH a value on the stack for a new process
//    -------------------------------------------
#define PUSH(tos,val) (*(-- tos) = val)


//
// -- This is the timeslice allowed befre preemption
//    ----------------------------------------------
#define TIMESLICE   1


//
// -- The number of times IRQs have been disbled
//    ------------------------------------------
static volatile int irqDisableCounter = 0;


//
// -- The time the CPU has been idle
//    ------------------------------
unsigned long idleCPUTime = 0;


//
// -- The number of times locks that would postpone task swaps have been obtained
//    ---------------------------------------------------------------------------
volatile int postponeTaskCounter = 0;


//
// -- Has a task change been postponed?
//    ---------------------------------
bool taskChangePostponedFlag = false;


//
// -- the implementation of the running task
//    --------------------------------------
PCB_t *currentPCB;


//
// -- The list of ready to run processes
//    ----------------------------------
PCB_t *readyListHead;
PCB_t *readyListTail;


//
// -- The list of sleeping tasks
//    --------------------------
PCB_t *sleepingListHead;
PCB_t *sleepingListTail;


//
// -- The list of terminated tasks
//    ----------------------------
PCB_t *terminatedListHead;
PCB_t *terminatedListTail;


//
// -- The Butler task
//    ---------------
PCB_t *butler;


//
// -- This is the array of possible running tasks
//    -------------------------------------------
PCB_t pcbArray[MAX_TASKS];


//
// -- This is the last counter value read
//    -----------------------------------
unsigned long lastCounter;


//
// -- initialize the current task block
//    ---------------------------------
void InitScheduler(void) 
{
    for (int i = 0; i < MAX_TASKS; i ++) pcbArray[i].used = 0;

    pcbArray[0].used = 1;
    pcbArray[0].tos = 0;
    pcbArray[0].virtAddr = GetCR3();
    pcbArray[0].state = RUNNING;
    pcbArray[0].next = (PCB_t *)0;
    pcbArray[0].quantumLeft = 0;

    currentPCB = &pcbArray[0];
    butler = currentPCB;

    readyListHead = (PCB_t *)0;
    readyListTail = (PCB_t *)0;

    sleepingListHead = (PCB_t *)0;
    sleepingListTail = (PCB_t *)0;

    terminatedListHead = (PCB_t *)0;
    terminatedListTail = (PCB_t *)0;

    lastCounter = GetCurrentCounter();
}


//
// -- allocate a PCB and return its address to the calling function
//    -------------------------------------------------------------
PCB_t *PcbAlloc(void)
{
    for (int i = 0; i < MAX_TASKS; i ++) {
        if (pcbArray[i].used == 0) {
            pcbArray[i].used = 1;
            pcbArray[i].tos = ((0x181 + i) << 12);  // also allocate a stack here

            return &pcbArray[i];
        }
    }

    return (PCB_t *)0;
}


//
// -- Free a PCB back to the pool
//    ---------------------------
void PcbFree(PCB_t *pcb)
{
    if (pcb < &pcbArray[0] || pcb >= &pcbArray[MAX_TASKS]) return;
    pcb->used = 0;
}


//
// -- Process startup function which will be called before the actual entry point
//    ---------------------------------------------------------------------------
static void ProcessStartup(void) 
{
    UnlockScheduler();
}


//
// -- Add a process to the list of ready tasks
//    ----------------------------------------
void AddReady(PCB_t *task)
{
    if (!task) return;

    task->state = READY;
    task->next = (PCB_t *)0;            // should already be null, but just in case

    if (readyListHead == (PCB_t *)0) {
        readyListHead = readyListTail = task;
    } else {
        readyListTail->next = task;
        readyListTail = task;
    }
}


//
// -- Get the next ready task, removing it from the list
//    WARNING: Might return NULL!!!
//    --------------------------------------------------
PCB_t *NextReady(void)
{
    if (readyListHead == (PCB_t *)0) return (PCB_t *)0;

    PCB_t *rv = readyListHead;
    readyListHead = readyListHead->next;

    if (readyListHead == (PCB_t *)0) readyListTail = readyListHead;

    rv->next = (PCB_t *)0;
    return rv;
}


//
// -- Create a new process with its entry point
//    -----------------------------------------
PCB_t *CreateProcess(void (*ent)())
{
    PCB_t *rv = PcbAlloc();

    if (!rv) return rv;

    unsigned int *tos = (unsigned int *)rv->tos;
    PUSH(tos, (unsigned int)ent);        // entry point
    PUSH(tos, (unsigned int)ProcessStartup);  // startup function
    PUSH(tos, 0);           // EBP
    PUSH(tos, 0);           // EDI
    PUSH(tos, 0);           // ESI
    PUSH(tos, 0);           // EBX

    rv->tos = (unsigned int)tos;
    rv->virtAddr = GetCR3();
    rv->sleepUntil = (unsigned long)-1;
    rv->quantumLeft = 0;
    // strcpy(rv->current_path,getcwd());
    LockAndPostpone();
    AddReady(rv);
    UnlockAndSchedule();

    return rv;
}


//
// -- based on what is currently running (roun-robin style), select the next task
//
//    !!!! The caller must have called LockScheduler() and must call !!!!
//    !!!! UnlockScheduler() before and after the call to Schedule() !!!!
//    ---------------------------------------------------------------------------
void Schedule(void) 
{
    if (postponeTaskCounter != 0) {
        taskChangePostponedFlag = true;
        return;
    }

    PCB_t *next = NextReady();

    if (next) {
        next->quantumLeft = TIMESLICE;
            // STI();
        // chdir(next->current_path);
        SwitchToTask(next);
    } else if (currentPCB->state == RUNNING) {
        return;
    } else {
        PCB_t *task = currentPCB;
        currentPCB = (PCB_t *)0;

        // -- wait for the timer to make something ready to run
        do {
            STI();
            HLT();
            CLI();
            next = NextReady();
        } while (next == (PCB_t *)0);

        currentPCB = task;
        next->quantumLeft = TIMESLICE;
        if (next != currentPCB) {
            // STI();
            // chdir(next->current_path);
            SwitchToTask(next);
        }
    }
}


//
// -- Update the current process with the amount of time used
//    -------------------------------------------------------
void UpdateTimeUsed(void)
{
    unsigned long c = lastCounter;
    lastCounter = GetCurrentCounter();

    if (currentPCB == (PCB_t *)0) {
        idleCPUTime += (lastCounter - c);
    } else {
        currentPCB->used += (lastCounter - c);
    }
}


//
// -- Lock the scheduler
//    ------------------
void LockScheduler(void) 
{
    CLI();
    irqDisableCounter ++;
}


//
// -- Unlock the scheduler
//    --------------------
void UnlockScheduler(void)
{
    irqDisableCounter --;
    if (irqDisableCounter == 0) STI();
}


//
// -- Obtain a lock (global for now) and postpone task changes
//    --------------------------------------------------------
void LockAndPostpone(void) 
{
    LockScheduler();
    postponeTaskCounter ++;
}


//
// -- Unlock a lock (global for now) and perform a Schedule if we can
//    ---------------------------------------------------------------
void UnlockAndSchedule(void)
{
    postponeTaskCounter --;
    if (postponeTaskCounter == 0) {
        if (taskChangePostponedFlag) {
            taskChangePostponedFlag = false;
            Schedule();
        }
    }

    UnlockScheduler();
}



//
// -- Block the current process
//    -------------------------
void BlockProcess(int reason)
{
    LockAndPostpone();
    currentPCB->state = reason;
    Schedule();
    UnlockAndSchedule();
}


//
// -- Unblock a Process
//    -----------------
void UnblockProcess(PCB_t *proc) 
{
    LockAndPostpone();
    AddReady(proc);
    UnlockAndSchedule();
}


//
// -- Add a process to the list of sleeping tasks
//    -------------------------------------------
void AddSleeping(PCB_t *task)
{
    if (!task) return;

    task->state = SLEEPING;

    if (sleepingListHead == (PCB_t *)0) {
        sleepingListHead = sleepingListTail = task;
    } else {
        sleepingListTail->next = task;
        sleepingListTail = task;
    }
}


//
// -- Sleep until time has passed
//    ---------------------------
void SleepUntil(unsigned long when)
{
    LockAndPostpone();

    if (when < GetCurrentCounter()) {
        UnlockAndSchedule();
        return;
    }

    currentPCB->sleepUntil = when;
    AddSleeping(currentPCB);

    UnlockAndSchedule();    // -- this is OK because the scheduler structures are in order; worst case 
                            //    is a task change to itself.
    BlockProcess(SLEEPING);
}


//
// -- This is the timer handler -- called with every timer tick
//    Schedule() must be postponed before calling this function
//    ---------------------------------------------------------
void IrqTimerHandler(void)
{
    counter ++;

    PCB_t *oldSleeping = sleepingListHead;
    PCB_t *work = oldSleeping;
    sleepingListHead = sleepingListTail = (PCB_t *)0;

    while (work != (PCB_t *)0) {
        oldSleeping = work->next;
        work->next = (PCB_t *)0;

        if (work->sleepUntil <= counter) {
            UnblockProcess(work);
        } else {
            AddSleeping(work);
        }

        work = oldSleeping;
    }

    if (currentPCB) {
        currentPCB->quantumLeft --;

        if (currentPCB->quantumLeft <= 0) {
            Schedule();
        }
    }
}


//
// -- Add a process to the list of terminated tasks
//    ---------------------------------------------
void AddTerminated(PCB_t *task)
{
    if (!task) return;

    task->state = SLEEPING;

    if (terminatedListHead == (PCB_t *)0) {
        terminatedListHead = terminatedListTail = task;
    } else {
        terminatedListTail->next = task;
        terminatedListTail = task;
    }
}


//
// -- Terminate a task
//    ----------------
void TerminateProcess(void)
{
    LockAndPostpone();
    AddTerminated(currentPCB);
    BlockProcess(TERMINATED);       // process changes postponed
    UnblockProcess(butler);

    UnlockAndSchedule();            // The last thing executed as this process
}


//
// -- Perform the Butler Responsibilities
//    -----------------------------------
void PerformButler(void)
{
    while (true) {
        LockAndPostpone();
        if (terminatedListHead == (PCB_t *)0) {
            BlockProcess(PAUSED);
            UnlockAndSchedule();
            continue;
        }

        PCB_t *work = terminatedListHead;
        terminatedListHead = terminatedListHead->next;
        if (terminatedListHead == (PCB_t *)0) terminatedListTail = terminatedListHead;

        PcbFree(work);

        UnlockAndSchedule();
    }
}
PCB_t *GetCurrentTask(void) {
    return currentPCB;
}