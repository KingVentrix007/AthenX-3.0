;;===================================================================================================================
;;
;;  timer-handler.s -- This is the timer ISR routine, which will handle keeping the counter up to date
;;
;; -----------------------------------------------------------------------------------------------------------------
;;
;;     Date      Tracker  Version  Pgmr  Description
;;  -----------  -------  -------  ----  --------------------------------------------------------------------------
;;  2019-Sep-29  Initial   step03  ADCL  Initial version
;;  2019-Nov-05  Step 9   step08   ADCL  Add sleeping to the process repetoire
;;
;;===================================================================================================================


;;
;; -- things that need to be seen outside this source
;;    -----------------------------------------------
        global      TimerHandler


;;
;; -- things that need to be seen here but are outside this source
;;    ------------------------------------------------------------
        extern      counter
        extern      IrqTimerHandler
        extern      LockAndPostpone
        extern      UnlockAndSchedule
        extern      WriteChar

;; 
;; -- This is the actual entry point
;;    ------------------------------
        section     .text
TimerHandler:
        inc         dword [counter]

;;
;; -- issue an EOI
;;    ------------
        ;; -- push all registers
        push        eax
        push        ebx
        push        ecx
        push        edx
        push        ebp
        push        esi
        push        edi

        mov         eax,0x20
        out         0x20,al

        call        LockAndPostpone
        call        IrqTimerHandler
        call        UnlockAndSchedule

        ;; -- restore all registers
        pop         edi
        pop         esi
        pop         ebp
        pop         edx
        pop         ecx
        pop         ebx
        pop         eax

        iret
