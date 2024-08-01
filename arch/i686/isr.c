#include "isr.h"
#include "idt.h"
#include "8259_pic.h"
#include "debug_term.h"
#include "debug.h"
#include "io_ports.h"
#include "cpu.h"
#include "keyboard.h"
#include "scheduler.h"
#include "string.h"
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
    // LockScheduler();
    if (g_interrupt_handlers[reg->int_no] != NULL) {
        ISR handler = g_interrupt_handlers[reg->int_no];
        handler(reg);
    }
    else
    {
        dbgprintf("Something tried to interrupt %d\n", reg->int_no);
    }
    pic8259_eoi(reg->int_no);
    // UnlockScheduler();
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

int debug_c(char *cmd)
{
    if(strcmp(cmd,"dec") == 0)
    {
        cls();
        for (size_t i = 0; i < num_found_functions; i++)
        {
            FunctionInfo *info  = &found_functions[i];
            printf("Function %s decompiled\n",info->function_name);
            print_stack_frame((uintptr_t*)info->func_address, sizeof(uintptr_t) * 16,found_functions); // Example frame size, adjust as needed
            printf("\nPress the down arrow for the next function\n");
            int input = kb_getchar_w();
            while(input != SCAN_CODE_KEY_DOWN)
            {
                input = kb_getchar_w();
            }
            cls();
        }
        
        
    }
}

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
            printf("Debug terminal:\n");
            // STI();
            char cmd_buffer[1024];
            int num_chars = 0;
            UnlockScheduler();
            while(1==1)
            {
                printf("Debug>>");
                
                int input = kb_getchar_w();
                while(input != '\n')
                {
                    if(input == '\b' && num_chars > 0)
                    {
                        printf("\b");
                        cmd_buffer[num_chars] = '\0';
                        num_chars--;

                    }
                    else if (input != '\0')
                    {
                        printf("%c",input);
                        cmd_buffer[num_chars] = input;
                        num_chars++;
                    }
                    input = kb_getchar_w();

                    
                }
                debug_c(cmd_buffer);
                printf("\n");
                // printf("Command == [%s]\n",cmd_buffer);
                memset(cmd_buffer,0,1024);
                num_chars = 0;
            }
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