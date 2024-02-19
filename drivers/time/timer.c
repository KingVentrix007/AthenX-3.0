#include "timer.h"

#include "printf.h"
#include "idt.h"
#include "io_ports.h"
#include "isr.h"
#include "string.h"
#include "types.h"
#include "scheduler.h"
// number of ticks since system booted
uint32 g_ticks = 0;
// frequency in hertz
uint16 g_freq_hz = 0;
// timer functions to be called when that ticks reached in irq handler
TIMER_FUNCTION_MANAGER g_timer_function_manager;


// See https://wiki.osdev.org/Programmable_Interval_Timer
void timer_set_frequency(uint16 f) {
    g_freq_hz = f;
    uint16 divisor = TIMER_INPUT_CLOCK_FREQUENCY / f;
    // set Mode 3 - Square Wave Mode
    outportb(TIMER_COMMAND_PORT, 0b00110110);
    // set low byte
    outportb(TIMER_CHANNEL_0_DATA_PORT, divisor & 0xFF);
    // set high byte
    outportb(TIMER_CHANNEL_0_DATA_PORT, (divisor >> 8) & 0xFF);
}


void timer_handler(REGISTERS* r) {
    uint32 i;
    TIMER_FUNC_ARGS *args = NULL;
    g_ticks++;
    counter++;
    //printf("timer triggered at frequency %d\n", g_ticks);
    for (i = 0; i < MAXIMUM_TIMER_FUNCTIONS; i++) {
        args = &g_timer_function_manager.func_args[i];
        if (args->timeout == 0)
            continue;
        if ((g_ticks % args->timeout) == 0) {
            g_timer_function_manager.functions[i](args);
        }
    }
    //  printf("Timer\n");
    // isr_end_interrupt(r->int_no);
    // UnlockScheduler();
     pic8259_eoi(r->int_no);
     LockAndPostpone();
        IrqTimerHandler();
     UnlockAndSchedule();
     
    // printf("hello\n");
    // switchTask();
}

void timer_register_function(TIMER_FUNCTION function, TIMER_FUNC_ARGS *args) {
    uint32 index = 0;
    if (function == NULL || args == NULL) {
        printf("ERROR: failed to register timer function %x\n", function);
        return;
    }
    index = (++g_timer_function_manager.current_index) % MAXIMUM_TIMER_FUNCTIONS;
    g_timer_function_manager.current_index = index;
    g_timer_function_manager.functions[index] = function;
    memcpy(&g_timer_function_manager.func_args[index], args, sizeof(TIMER_FUNC_ARGS));
}

void timer_init() {
    // IRQ0 will fire 100 times per second
    // initMultitasking();
   
    timer_set_frequency(100);
    isr_register_interrupt_handler(IRQ_BASE, timer_handler);
}

void sleep(int sec) {
    uint32 end = g_ticks + sec * g_freq_hz;
    while (g_ticks < end);
}
uint32 get_ticks()
{
    return g_ticks;
}
// void TimerCallBack(REGISTERS *r)
// {
//     uint64_t now = 0;

//     TimerEoi(timerControl);     // take care of this while interrupts are disabled!

//     ProcessLockAndPostpone();

// #if DEBUG_ENABLED(TimerCallBack)
//     kprintf("handling timer\n");
// #endif

//     if (timerControl->TimerPlatformTick && thisCpu->cpuNum == 0) TimerPlatformTick(timerControl);

//     //
//     // -- here we look for any sleeping tasks to wake
//     //    -------------------------------------------
//     now = TimerCurrentCount(timerControl);
//     if (now >= scheduler.nextWake && IsListEmpty(&scheduler.listSleeping) == false) {
//         uint64_t newWake = (uint64_t)-1;


//         //
//         // -- loop through and find the processes to wake up
//         //    ----------------------------------------------
//         ListHead_t::List_t *list = scheduler.listSleeping.list.next;
//         while (list != &scheduler.listSleeping.list) {
//             ListHead_t::List_t *next = list->next;      // must be saved before it is changed below
//             Process_t *wrk = FIND_PARENT(list, Process_t, stsQueue);
//             if (now >= wrk->wakeAtMicros) {
//                 wrk->wakeAtMicros = 0;
//                 ListRemoveInit(&wrk->stsQueue);
//                 ProcessDoUnblock(wrk);
//             } else if (wrk->wakeAtMicros < newWake) newWake = wrk->wakeAtMicros;

//             list = next;
//         }

//         scheduler.nextWake = newWake;
//     }


//     //
//     // -- adjust the quantum and see if it is time to change tasks
//     //    --------------------------------------------------------
//     if (currentThread != NULL) {
//         if (AtomicDec(&currentThread->quantumLeft) <= 0) {
// // #if DEBUG_ENABLED(TimerCallBack)
// //             kprintf("Preempt\n");
// // #endif
//             scheduler.processChangePending = true;
//         }
//     }

//     ProcessUnlockAndSchedule();
// }