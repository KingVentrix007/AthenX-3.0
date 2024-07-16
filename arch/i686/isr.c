#include "isr.h"
#include "idt.h"
#include "8259_pic.h"
#include "debug_term.h"
#include "debug.h"
#define MAX_EXCEPTIONS 10
#define EOI(irq) \
    do {\
        if (irq >= 0x28){\
		    outportb(PIC2, 0x20); /* Slave */\
        } \
        outportb(PIC1, 0x20); /* Master */\
    } while(0);
// For both exceptions and irq interrupt
ISR g_interrupt_handlers[NO_INTERRUPT_HANDLERS];

// for more details, see Intel manual -> Interrupt & Exception Handling
char *exception_messages[32] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "BOUND Range Exceeded",
    "Invalid Opcode",
    "Device Not Available (No Math Coprocessor)",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection",
    "Page Fault",
    "Unknown Interrupt (intel reserved)",
    "x87 FPU Floating-Point Error (Math Fault)",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

/**
 * register given handler to interrupt handlers at given num
 */
void isr_register_interrupt_handler(int num, ISR handler,char func[255]) {
    printf("IRQ %d registered for %s\n", num,func);
    if (num < NO_INTERRUPT_HANDLERS)
        if(g_interrupt_handlers[num] == NULL)
        {
            g_interrupt_handlers[num] = handler;
        }
        else
        {
            printf("Attempted to register IRQ %d a second time for %s\n", num,func);
        }
        
}

/*
 * turn off current interrupt
*/
void isr_end_interrupt(int num) {
    pic8259_eoi(num);
}

/**
 * invoke isr routine and send eoi to pic,
 * being called in irq.asm
 */
void isr_irq_handler(REGISTERS *reg) {
    // printf("IRQ\n");
    if (g_interrupt_handlers[reg->int_no] != NULL) {
        if(reg->int_no == 43)
        {
            printf("Calling interrupt handler for e1000\n");
        }
        ISR handler = g_interrupt_handlers[reg->int_no];
        handler(reg);
    }
    else
    {
        printf_com("Something tried to interrupt %d\n", reg->int_no);
    }
    if(reg->int_no != 32)
		EOI(reg->int_no);
}
uint32_t *unwind_stack(REGISTERS *reg);
static void print_registers(REGISTERS *reg) {
    cls();
    printf("REGISTERS:\n");
    printf("err_code=%d\n", reg->err_code);
    printf("eax=0x%x, ebx=0x%x, ecx=0x%x, edx=0x%x\n", reg->eax, reg->ebx, reg->ecx, reg->edx);
    printf("edi=0x%x, esi=0x%x, ebp=0x%x, esp=0x%x\n", reg->edi, reg->esi, reg->ebp, reg->esp);
    printf("eip=0x%x, cs=0x%x, ss=0x%x, eflags=0x%x, useresp=0x%x\n", reg->eip, reg->ss, reg->eflags, reg->useresp);

    // struct stackframe *frame_ptr;
    // asm ("movl %%ebp, %0" : "=r"(frame_ptr));  // Capture current frame pointer using inline assembly
    
}
void debug_output(REGISTERS *reg)
{
    printf("\033[1;31m => ");
    printf("Error %s\n",exception_messages[reg->int_no]);
    printf("\033[0m");
    // parse_elf32("/boot/AthenX.bin");
    // printf("Initial EBP = %p\n", frame_ptr);
    unwind_stack(reg);
    printf("Stack strace complete\n");
}
int exception_count = 0;

/**
 * invoke exception routine,
 * being called in exception.asm
 */
void isr_exception_handler(REGISTERS reg) {
    if (reg.int_no < 32) {
        LockScheduler();
        if(exception_count < MAX_EXCEPTIONS)
        {
            
            exception_count+=1;
            print_registers(&reg);
            debug_output(&reg);
            for (;;);
        }
        else
        {
            kernel_panic(__func__,"An exception loop accrued.");
        }
        //printf("EXCEPTION: %s\n", exception_messages[reg.int_no]);
        
    }
    if (g_interrupt_handlers[reg.int_no] != NULL) {
        ISR handler = g_interrupt_handlers[reg.int_no];
        handler(&reg);
    }
}