;;===================================================================================================================
;;
;;  SwitchToTask.s -- This will perform the actual task switch
;;
;; -----------------------------------------------------------------------------------------------------------------
;;
;;     Date      Tracker  Version  Pgmr  Description
;;  -----------  -------  -------  ----  --------------------------------------------------------------------------
;;  2019-Sep-22  Initial   step01  ADCL  Initial version
;;  2019-Sep-26  Step 01   step01  ADCL  Fix a stack symmetry issue
;;  2019-Oct-05  Step 3    step03  ADCL  Add the call to `UpdateTimeUsed()`
;;  2019-Oct-05  Step 4    step04  ADCL  Create a process state
;;  2019-Oct-24  Step 5    step05  ADCL  Add rudamentary scheduler lock
;;  2019-Oct-25  Step 6    step06  ADCL  Add the ability to block/unblock
;;  2019-Nov-01  Step 8    step08  ADCL  Add checks for postponed task changes
;;  2019-Nov-05  Step 9    step09  ADCL  Add sleeping to the process repetoire
;;  2019-Nov-20  Step12    step12  ADCL  Add process termination
;;
;;===================================================================================================================


;;
;; -- things that need to be seen outside this source
;;    -----------------------------------------------
        global      SwitchToTask        ;; void SwitchToTask(PCB_t *task) __attributes__((cdecl));
        global      GetCR3              ;; unsigned int GetCR3(void);


;;
;; -- thing that need to be see inside this source but are defined outside
;;    --------------------------------------------------------------------
        extern      currentPCB
        extern      UpdateTimeUsed
        extern      AddReady
        extern      postponeTaskCounter
        extern      taskChangePostponedFlag


;;
;; -- Some offsets into the PCB sructure
;;    ----------------------------------
TOS     equ         0
VAS     equ         4
STATE   equ         12


;;
;; -- These are the different states
;;    ------------------------------
RUNNING equ         0
READY   equ         1
PAUSED  equ         2
SLEEPING    equ     3
TERMINATED  equ     4


;;
;; -- This is the beginning of the code segment for this file
;;    -------------------------------------------------------
        section     .text
        cpu         586


;;
;; -- Perform the actual task switch
;;   
;;    Some notes about this function:
;;    * cdecl ensures that `eax`, `ecx`, and `edx` are saved on the stack
;;    * `eip` was also saved by the `call` instruction
;;    * `cr3` does not need to be saved -- once set it cannot be changed
;;    * segment registers do not need to be saves -- kernel segments to kernel segments
;;    
;;    IRQs are required to be disabled prior to calling and restored on exit
;;
;;    !!!! The caller must have called LockScheduler() and must call !!!!
;;    !!!! UnlockScheduler() before and after the call to Schedule() !!!!
;;    ---------------------------------------------------------------------------------
SwitchToTask:
        cmp         dword [postponeTaskCounter],0   ;; are we OK to change tasks?
        je          .swap
        mov         dword [taskChangePostponedFlag],1   ;; note we are ready to postpone tasks
        ret

.swap:
        push        ebx
        push        esi
        push        edi
        push        ebp

        mov         edi,[currentPCB]        ;; `edi` = previous tasks PCB

        cmp         dword [edi+STATE],RUNNING     ;; is the task RUNNING?
        jne         .noReady                ;; if not, no need to ready it

        push        edi                     ;; push the current process onto the stack
        call        AddReady                ;; add it to the ready list
        add         esp,4                   ;; drop the value from the stack

.noReady:
        call        UpdateTimeUsed          ;; update the time used field before swapping

        mov         [edi+TOS],esp           ;; save the top of the stack


        ;; -- load the next task's state


        mov         esi,[esp+((4+1)*4)]     ;; `esi` = next tasks PCB
        mov         [currentPCB],esi        ;; save the current process PCB

        mov         esp,[esi+TOS]           ;; load the next process's stack
        mov         eax,[esi+VAS]           ;; load the next process's virtual address space
        mov         dword [esi+STATE],RUNNING     ;; make the current task running

        ;; -- fix tss.esp0 here

        mov         ecx,cr3                 ;; get the current task cr3

        cmp         ecx,eax                 ;; are the virtual address spaces the same
        jmp         .sameVAS                
        mov         cr3,eax                 ;; update to the new virtual address space

.sameVAS:
        pop         ebp
        pop         edi
        pop         esi
        pop         ebx

        ret                                 ;; this is the next task's `eip`


;;
;; -- get the current `cr3` value
;;    ---------------------------
GetCR3:
        mov         eax,cr3
        ret