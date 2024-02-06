//===================================================================================================================
//
//  scheduler.h -- Strutures and prototypes for takng care of task scheduling
//
// -----------------------------------------------------------------------------------------------------------------
//
//     Date      Tracker  Version  Pgmr  Description
//  -----------  -------  -------  ----  --------------------------------------------------------------------------
//  2019-Sep-21  Initial  step01   ADCL  Initial version
//  2019-Sep-25  Step 2   step02   ADCL  Add `Schedule()`
//  2019-Oct-05  Step 3   step03   ADCL  Add a call to `UpdateTimeUsed()`
//  2019-Oct-05  Step 4   step04   ADCL  Create a process state
//  2019-Oct-24  Step 5   step05   ADCL  Add rudamentary scheduler lock
//  2019-Oct-25  Step 6   step06   ADCL  Add the ability to block/unblock
//  2019-Nov-05  Step 9   step09   ADCL  Add sleeping to the process repetoire
//  2019-Nov-09  Step10   step10   ADCL  Add idle CPU handling (and clean up the time used)
//  2019-Nov-10  Step11   step11   ADCL  Add preemption
//  2019-Nov-20  Step12   step12   ADCL  Add process termination
//
//===================================================================================================================


#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__
#include "cpu.h"

//
// -- Process States
//    --------------
typedef enum {
    RUNNING = 0,
    READY = 1,
    PAUSED = 2,
    SLEEPING = 3,
    TERMINATED = 4,
} ProcessState_t;



//
// -- This is the Process Control Block -- for controlling processes by the scheduler
//    -------------------------------------------------------------------------------
typedef struct PCB_t {
    unsigned int tos;
    unsigned int virtAddr;
    struct PCB_t *next;
    int state;
    unsigned long used;
    unsigned long sleepUntil;
    int quantumLeft;
    char current_path[100];
    
} PCB_t;


//
// -- We need to keep track of the current task
//    -----------------------------------------
extern PCB_t *currentPCB;


//
// -- This is the head/tail list of ready to run processes
//    ----------------------------------------------------
extern PCB_t *readyListHead;
extern PCB_t *readyListTail;


//
// -- an array of PCBs that can be used
//    ---------------------------------
#define MAX_TASKS       256
extern PCB_t pcbArray[MAX_TASKS];


//
// -- Funtion prototypes
//    ------------------

    void SwitchToTask(PCB_t *task) __attribute__((cdecl));
    unsigned int GetCR3(void);

    void AddReady(PCB_t *task);
    PCB_t *NextReady(void);
    void UpdateTimeUsed(void);

    void InitScheduler(void);
    void LockScheduler(void); 
    void UnlockScheduler(void);
    void LockAndPostpone(void);
    void UnlockAndSchedule(void);

    void Schedule(void);
    void IrqTimerHandler(void);

    PCB_t *CreateProcess(void (*ent)());
    void BlockProcess(int reason);
    void UnblockProcess(PCB_t *proc);

    void SleepUntil(unsigned long when);

    void TerminateProcess(void);
    void PerformButler(void);
    PCB_t *GetCurrentTask(void);


inline void Sleep(unsigned long millis) { SleepUntil(millis + GetCurrentCounter()); }

#endif

