//===================================================================================================================
//
//  cpu.c -- Maintain the CPU structures in the CPU
//
// -----------------------------------------------------------------------------------------------------------------
//
//     Date      Tracker  Version  Pgmr  Description
//  -----------  -------  -------  ----  --------------------------------------------------------------------------
//  2019-Sep-29  Initial  step03   ADCL  Initial version
//  2019-Nov-05  Step 9   step08   ADCL  Fix bug with PIC initialization
//
//===================================================================================================================


#include "cpu.h"


//
// -- This is the timer counter; recall that this is in the .bss section and is not initialized
//    -----------------------------------------------------------------------------------------
volatile unsigned long counter;


// 
// -- Set an idt entry to the service routine
//    ---------------------------------------
static void SetIdtEntry(int e, void *a, unsigned int f) 
{
    // IdtEntry_t *ent = &idtTable[e];
    // unsigned int addr = (unsigned int)a;

    // // -- sanity check
    // if (e < 0 || e >= 256) return;

    // ent->baseLow = (addr & 0xffff);
    // ent->baseHigh = (addr >> 16) & 0xffff;
    // ent->sel = 0x08;
    // ent->always0 = 0;
    // ent->flags = (0x8e | f);
}



//
// -- Initialize the timer with everything we need to operate
//    -------------------------------------------------------
void InitTimer(void)
{
    // -- PIC init
    // OUTB(0x21, 0xff);            // Disable all IRQs
    // OUTB(0xa1, 0xff);             // Disable all IRQs
    // OUTB(0x20, 0x11);
    // OUTB(0xa0, 0x11);
    // OUTB(0x21, 0x20);
    // OUTB(0xa1, 0x28);
    // OUTB(0x21, 0x04);
    // OUTB(0xa1, 0x02);
    // OUTB(0x21, 0x01);
    // OUTB(0xa1, 0x01);

    // // -- Register handler
    // SetIdtEntry(0x20, (void *)TimerHandler, 0x00);

    // // -- PIT init
    // unsigned long divisor = 1193180 / 1000;     // -- or 1193
    // unsigned char l = (unsigned char)(divisor & 0xff);
    // unsigned char h = (unsigned char)((divisor >> 8) & 0xff);

    // OUTB(0x43, 0x36);
    // OUTB(0x40, l);
    // OUTB(0x40, h);

    // OUTB(0x21, 0x00);            // Enable all IRQs
    // OUTB(0xa1, 0x00);             // Enable all IRQs
}


//
// -- get the current counter value
//    -----------------------------
unsigned long GetCurrentCounter(void) 
{
    return counter;
}
