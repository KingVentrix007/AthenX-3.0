//===================================================================================================================
//
//  cpu.h -- Structures that are needed by the CPU that we can utilize from C(++)
//
// -----------------------------------------------------------------------------------------------------------------
//
//     Date      Tracker  Version  Pgmr  Description
//  -----------  -------  -------  ----  --------------------------------------------------------------------------
//  2019-Sep-29  Initial  step03   ADCL  Initial version
//
//===================================================================================================================


#ifndef __CPU_H__
#define __CPU_H__


//
// -- The Interrupt Descriptor Table Entry
//    ------------------------------------
typedef struct IdtEntry_t {
    unsigned int baseLow : 16;
    unsigned int sel : 16;
    unsigned int always0 : 8;
    unsigned int flags : 8;
    unsigned int baseHigh : 16;
} IdtEntry_t;


//
// -- this is the pointer to the IDT Table for maintenance
//    ----------------------------------------------------
// IdtEntry_t * const idtTable = (IdtEntry_t *)(0x17f000);


//
// -- This is the cpu counter (at 1ms resolution)
//    -------------------------------------------
volatile extern unsigned long counter;


//
// -- The function prototypes
//    -----------------------

    void InitTimer(void);
    void TimerHandler(void);
    unsigned long GetCurrentCounter(void);



//
// -- read a byte from a hardware port
//    --------------------------------
#define INB(port) ({                                                    \
    unsigned char _byte;                                                \
    __asm__ volatile("inb %1,%b0" : "=a"(_byte) : "Nd"(port));           \
    _byte;                                                              \
})


//
// -- write a byte to a hardware port
//    -------------------------------
#define OUTB(port,val) ({                                               \
    __asm__ volatile("outb %b0,%1" : : "a"(val), "Nd"(port));           \
})


//
// -- Enable interrupts
//    -----------------
#define STI()           ({                                              \
    __asm__ volatile("sti");                                            \
})


//
// -- Disable interrupts
//    ------------------
#define CLI()           ({                                              \
    __asm__ volatile("cli");                                            \
})


//
// -- Halt the cpu
//    ------------
#define HLT()           ({                                              \
    __asm__ volatile("hlt");                                            \
})


#endif