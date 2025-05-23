#include <elf.h>
#include "../include/fat_filelib.h"
// #include "mem.h"
#include "stdbool.h"
#include "string.h"
#include "../include/maths.h"
#include "../include/exe.h"
#include "../include/i386.h"
#include "../include/kernel.h"
// #include "../include/debug.h"
#include "../include/isr.h"
// #include "../include/x86_reg.h"
#include "printf.h"
#include "vmm.h"
#include "kheap.h"
#include "scheduler.h"
#include "keyboard.h"
#include "io_ports.h"
void internal_run_exe();
// #include "../include/sys_handler.h"
#define PT_GNU_STACK	(PT_LOOS + 0x474e551)
addr original_esp;
addr original_ebp;

uint8_t process_stack[8192] __attribute__((aligned(16)));
// Define a structure to represent the user process context
typedef struct {
    uint32_t esp;
    uint32_t eip;
} UserProcessContext;
typedef struct {
    uint32_t esp;
} KernelContext;
// Define a structure to store register values
typedef struct {
    uint32_t ebp;
    uint32_t esp;
} RegisterState;



struct elf_exe {
    uint8_t* elf_start; // Pointer to the start address of the ELF file
    int priority;       // Priority number (not used in this code)
    uint32_t stack_size; // Size of the stack
    // Add any other necessary members here
};





char global_file_path[1000];
int global_argc;
char **global_argv;

struct elf_exe executable_file;
/**
 * Function Name: run_elf
 * Description: Load and run an ELF executable.
 *
 * Parameters:
 *   elf - A struct containing the ELF file's start address and other data.
 *
 * Return:
 *   [Description of the return value, if applicable]
 */
void run_elf(struct elf_exe elf, int myArgc,char* myArgv[]) {
    //printf("Running elf\n");
    // Define a stack aligned to 16 bytes
    uint8_t stack = process_stack;
    //printf("Initializing Stack\n");
    memset(stack, 0, 8192);
   
    // Set up the stack pointer (ESP) to point to the top of the stack
    asm volatile (
        "mov %0, %%esp" // Set the stack pointer
        :
        : "r"(&stack)
    );
    //  //printf("Initialzed stack\n");
    // Define the entry pointer as a function
    
    void (*elf_entry)(int, char **) = (void (*)(int, char **))(elf.elf_start); // Assuming ELF entry point is at offset 0
    //printf("Calling elf_entry point\n");
    // Call the ELF entry point function with the arguments
    elf_entry(myArgc, myArgv);
    //printf("Execution completed\n");
    int ret = 0;
    asm volatile(
        "movl %%eax, %0" // mov the top value from the stack and store it in ret
        : "=r" (ret) // Output operand
    );
    
    //printf("\nComplete ELF execution with exit code %d\n",ret);
    // Restore the stack pointer (ESP)
    asm volatile (
        "mov %0, %%esp"
        :
        : "r"(&stack)
    );
    // deallocate_untracked_memory();
}    















bool check_overlap(uintptr_t kernel_start, uintptr_t kernel_end, Elf32_Ehdr* elf_header) {
    uintptr_t elf_start = (uintptr_t)elf_header;
    uintptr_t elf_end = elf_start + elf_header->e_ehsize; // Adjust this as needed based on ELF structure.

    // Check for overlap by comparing the addresses.
    if (elf_end < kernel_start || elf_start > kernel_end) {
        // No overlap, return false.
        return false;
    } else {
        // Overlap detected, return true.
        return true;
    }
}

void save_register_state(RegisterState* state) {
    __asm__ volatile (
        "movl %%ebp, %0\n\t"  // Move the value of ebp into state->ebp
        "movl %%esp, %1\n\t"  // Move the value of esp into state->esp
        : "=r" (state->ebp), "=r" (state->esp)  // Output operands
        :
        : "memory"
    );
}
void restore_register_state(RegisterState* state) {
    __asm__ volatile (
        "movl %0, %%ebp\n\t"  // Move the value in state->ebp into ebp
        "movl %1, %%esp\n\t"  // Move the value in state->esp into esp
        :
        : "r" (state->ebp), "r" (state->esp)  // Input operands
        : "memory"
    );
}

// Define memory layout constants
#define KERNEL_START_ADDR   0x1000  // Adjust as needed
#define KERNEL_END_ADDR     0x5000  // Adjust as needed


// Define memory layout constants for the ELF file
#define ELF_LOAD_OFFSET   0x9000  // Adjust this offset as needed

// Function to check if a segment overlaps with kernel memory
int is_kernel_memory_overlap(Elf32_Phdr* segment) {
    uint32_t segment_start = segment->p_vaddr;
    uint32_t segment_end = segment_start + segment->p_memsz;

    // Calculate the load address based on ELF_LOAD_OFFSET
    uint8_t* load_address = (uint8_t*)((uint32_t)&__kernel_section_end + ELF_LOAD_OFFSET);
    ////printf("load->%p\n",load_address);
    // Check for overlap with kernel memory region
    uint8_t* kernel_start = &__kernel_section_start;
    uint8_t* kernel_end = &__kernel_section_end;

    if (segment_start < (uint32_t)kernel_end && segment_end > (uint32_t)kernel_start) {
        // Handle overlap error (e.g., reject the load)
        //printf("Error: ELF segment overlaps with kernel memory\n");
        //printf("Segment Start: %p\n", (void*)segment_start);
        //printf("Segment End: %p\n", (void*)segment_end);
        //printf("Kernel Start: %p\n", (void*)kernel_start);
        //printf("Kernel End: %p\n", (void*)kernel_end);
        return 1;  // Overlap detected
    }

    return 0;  // No overlap
}
void switch_to_user_mode(UserProcessContext *user_context, KernelContext *kernel_context) {
    // FUNC_ADDR_NAME(&switch_to_user_mode,2,"uu");
    // Save the kernel context
    asm volatile (
        "movl %%esp, %0"
        : "=r" (kernel_context->esp)
    );
     //printf("here");
    // Load the user process context
    asm volatile (
        "mov %0, %%esp \n"  // Load ESP
       
        "call *%1"          // Call the entry point
        :
        : "r" (user_context->esp), "r" (user_context->eip)
    );
    //printf("\nhere2");
    // Restore the kernel context
    asm volatile (
        "mov %0, %%esp"
        :
        : "r" (kernel_context->esp)
    );
}
// Function to load and execute an ELF executable

uintptr_t va_address[100];
uintptr_t pa_address[100];
int va_count = 0;
int pa_count = 0;
/**
 * Function Name: load_elf_executable
 * Description: Loads an ELF executable into memory, executes it, and deallocates allocated memory.
 *
 * Parameters:
 *   elf_data (uint8_t*) - Pointer to the ELF data
 *   myArgc (int) - Number of arguments for the ELF program
 *   myArgv (char**) - Array of argument strings for the ELF program
 *
 * Return:
 *   void
 */
void load_elf_executable(uint8_t* elf_data, int myArgc, char** myArgv) {
    printf("Passing parmas:\n");
    for (size_t i = 0; i < myArgc; i++)
    {
        printf("%d:%s\n",i,myArgv[i]);
    }
    
    Elf32_Ehdr* elf_header = (Elf32_Ehdr*)elf_data;
    LOG_LOCATION;
    if (memcmp(elf_header->e_ident, ELFMAG, SELFMAG) != 0) {
        return; // Not an ELF file
    }
     LOG_LOCATION;
    // printf("Num Segments: %d\n", elf_header->e_phnum);
    for (int i = 0; i < elf_header->e_phnum; i++) {
        Elf32_Phdr* program_header = (Elf32_Phdr*)(elf_data + elf_header->e_phoff + i * elf_header->e_phentsize);
         LOG_LOCATION;
        if (program_header->p_type == PT_LOAD) {
            uint32_t *region = kmalloc(program_header->p_filesz); // Allocate memory
            if (region == NULL) {
                return; // Failed to allocate memory
            }
             LOG_LOCATION;
            // va_address[pa_count] = region;
            // pa_count++;
            size_t num_pages = program_header->p_memsz / PAGE_SIZE;
            if (program_header->p_memsz % PAGE_SIZE != 0) {
                num_pages++;
            }
             LOG_LOCATION;
            uint32_t page_va = program_header->p_vaddr;
            uint32_t page_offset = program_header->p_vaddr & (program_header->p_align - 1);

            for (size_t i = 0; i < num_pages; i++) {
                printf("mapping address %p\n",page_va);
                // unmap(region);
                map(page_va, region, PAGE_WRITE | PAGE_PRESENT); // Map memory
                va_address[i] = page_va; 
                va_count++;
                page_va += PAGE_SIZE;
                region += PAGE_SIZE;
            }
            va_address[num_pages] = 0;
             LOG_LOCATION;
             printf("Memset below line %d\n",__LINE__);
            memcpy((void *)program_header->p_vaddr, elf_data + program_header->p_offset, program_header->p_filesz);
            printf("Memset below line %d\n",__LINE__);
            printf("Memset adder %p\n",(void *)(program_header->p_vaddr) + program_header->p_filesz);
            memset((void *)(program_header->p_vaddr) + program_header->p_filesz, 0, program_header->p_memsz - program_header->p_filesz);
             
             LOG_LOCATION;
        }
         LOG_LOCATION;

    }
     LOG_LOCATION;
    // Prepare to run elf program
    struct elf_exe my_elf;
    my_elf.elf_start = (uint32_t*)(elf_header->e_entry);

    // Initialize Stack
    uint8_t *stack = process_stack;
    printf("Memset below line %d\n",__LINE__);

    memset(stack, 0, 8192);
     LOG_LOCATION;
    // Set up the stack pointer (ESP)
    asm volatile (
        "mov %0, %%esp" // Set the stack pointer
        :
        : "r"(&stack)
    );
     LOG_LOCATION;
    void (*elf_entry)(int, char **) = (void (*)(int, char **))(elf_header->e_entry); // Assuming ELF entry point is at offset 0
    //  LOG_LOCATION;
    // Call the ELF entry point function with the arguments
    printf("Making entry\n");
    elf_entry(myArgc, myArgv);

    // Restore the stack pointer (ESP)
    asm volatile (
        "mov %0, %%esp"
        :
        : "r"(&stack)
    );
    LOG_LOCATION;
    printf("Memset below line %d\n",__LINE__);

    for (size_t i = 0; i < va_count; i++)
    {
        LOG_LOCATION;
        if(va_address[i] != 0)
        {
            memset((void *)va_address[i], 0, 4096);
            unmap(va_address[i]);
            va_address[i] = 0;
        }
       
    }
    va_count = 0;
    for (size_t i = 0; i < pa_count; i++)
    {
        LOG_LOCATION;
        kfree(pa_address[i]);
        pa_address[i] = 0;
    }
    pa_count = 0;
     LOG_LOCATION;
     return;
    // Deallocate memory
    // for (int i = 0; i < elf_header->e_phnum; i++) {
    //     Elf32_Phdr* program_header = (Elf32_Phdr*)(elf_data + elf_header->e_phoff + i * elf_header->e_phentsize);

    //     if (program_header->p_type == PT_LOAD) {
    //         // kfree((void *)(program_header->p_vaddr)); // Free allocated memory
    //         unmap(program_header->p_vaddr); // Unmap memory region
    //     }
    // }
}

// void load_elf_executable(uint8_t* elf_data,int myArgc,char **myArgv) {
//     // //printf("ARR = %s\n",myArgv[1]);
//     // FUNC_ADDR_NAME(&load_elf_executable,1,"u");
//     // Verify ELF magic number.
//     size_t stack_size = 0;
//     Elf32_Ehdr* elf_header = (Elf32_Ehdr*)elf_data;
    
//     if (elf_header->e_ident[EI_MAG0] != ELFMAG0 ||
//         elf_header->e_ident[EI_MAG1] != ELFMAG1 ||
//         elf_header->e_ident[EI_MAG2] != ELFMAG2 ||
//         elf_header->e_ident[EI_MAG3] != ELFMAG3) {
//         // Invalid ELF format.
//         // Handle error.
//         return;
//     }
    
//     // Read program headers.
//     Elf32_Phdr* program_headers = (Elf32_Phdr*)(elf_data + elf_header->e_phoff);
  
  
//     // bool over = check_overlap(__kernel_section_start,__kernel_section_end,elf_header);
//     // if(over)
//     // {
//     //     //printf("Overlap detected\n");
//     //     return -1;
//     // }
//     // else
//     // {
//     //     // //printf("Continuing execution\n");
//     // }
//     // Iterate through program headers and load loadable segments.
//     for (int i = 0; i < elf_header->e_phnum; i++) {
//         if (program_headers[i].p_type == PT_LOAD) {
//             // Calculate the load address by adding p_vaddr to __kernel_section_end
//             uint8_t* load_address = (uint8_t*)(uint32_t)(program_headers[i].p_vaddr);
//             uint32_t *physical_address = kmalloc(program_headers[i].p_filesz);
//             size_t num_pages = program_headers[i].p_memsz / PAGE_SIZE;
//             if (program_headers[i].p_memsz % PAGE_SIZE != 0) 
//             {
//                 num_pages++; // Increment if there's a partial page
//             }

//            for (size_t i = 0; i < num_pages; i++) 
//             {
//                 // Calculate the virtual address for this page
//                 uint32_t page_va = program_headers[i].p_vaddr + i * PAGE_SIZE;
//                 // Calculate the offset within the region for this page
//                 size_t page_offset = i * PAGE_SIZE;
//                 // Calculate the size of this page
//                 size_t page_size = (program_headers[i].p_memsz - page_offset) < PAGE_SIZE ? 
//                                 (program_headers[i].p_memsz - page_offset) : PAGE_SIZE;
//                 // Map this page
//                 map(page_va, physical_address + page_offset, program_headers[i].p_flags);
//             }
//             // Allocate memory and copy segment data.
//             uint8_t* file_data = elf_data + program_headers[i].p_offset;
//             uint32_t file_size = program_headers[i].p_filesz;

//             // Zero-fill any padding.
//             for (uint32_t j = program_headers[i].p_filesz; j < program_headers[i].p_memsz; j++) {
//                 load_address[j] = 0;
//             }

//             // Copy data.
//             memcpy(load_address, file_data, file_size);
//         }
//         if (program_headers[i].p_type == PT_GNU_STACK) {
//             stack_size = program_headers[i].p_memsz;
//             break;
//         }
//     }
//      struct elf_exe my_elf;
//     my_elf.elf_start = (uint32_t*)(elf_header->e_entry);
//     my_elf.stack_size = stack_size;
//     if (stack_size > 0) {
//         //printf("Stack size: %zu bytes\n", stack_size);
//     } else {
//         //printf("\n");
//         // //printf("No stack size information found in the ELF header\n");
//     }
//     //printf("\n----------------------------------------------------------------\n");
//     run_elf(my_elf,myArgc,myArgv);

//     return 0;
    
// }
void exit_elf(KernelContext* location)
{
     asm volatile (
        "mov %0, %%esp"
        :
        : "r" (location->esp)
    );
    // terminal_main();
}
void load_elf_file(const char* filename, int argc, char **argv) {
    // //printf("AR = %s\n",argv[1]);
    FL_FILE* file = fl_fopen(filename, "rb");
    if (file == NULL) {
        //printf("Failed to open file %s\n",filename);
        // Handle file opening error.
        return;
    }

    fl_fseek(file, 0, SEEK_END);
    long file_size = fl_ftell(file);
    fl_fseek(file, 0, SEEK_SET);
    //printf("Allocating memory for elf file of size %lu bytes\n", file_size);
    uint8_t* elf_data = (uint8_t*)kmalloc(file_size);
    if (elf_data == NULL) {
        //printf("Malloc error in elf\n");
        // Handle memory allocation error.
        fl_fclose(file);
        return;
    }
    ////printf("%d\n",__LINE__);
    fl_fread(elf_data, sizeof(uint8_t), file_size, file);
    fl_fclose(file);
    ////printf("%d\n",__LINE__);
    
    load_elf_executable(elf_data,argc, argv);
    LOG_LOCATION;
    // //printf("Exited to prime\n");
    ////printf("HERE");
    // Now, you can parse the ELF data and load it into memory as described in the previous responses.

    // Don't forget to kfree the allocated memory when you're done.
    kfree(elf_data);
    LOG_LOCATION;
}


// Function to set up the stack for the ELF program
// void setup_stack(void* stack, void* entry_point) {
//     // Initialize the stack pointer (ESP or RSP)
//     // Set it to the top of the allocated stack memory
//     asm volatile ("mov %0, %%esp" :: "r"(stack));  // For x86
//     // asm volatile ("mov %0, %%rsp" :: "r"(stack)); // For x86_64

//     // Push the entry point address onto the stack
//     asm volatile ("push %0" :: "r"(entry_point));
// }

// // Function to load and execute an ELF program
// void load_elf_file(const char* elf_file) {
//     // Open and read the ELF file
//     FL_FILE* file = fl_fopen(elf_file, "rb");
//     if (!file) {
//         perror("Failed to open file");
//         return 1;
//     }

//     // Read and verify the ELF header
//     Elf32_Ehdr elf_header;
//     if (fl_fread(&elf_header, sizeof(Elf32_Ehdr), 1, file) != 1) {
//         perror("Failed to read ELF header");
//         fl_fclose(file);
//         return 1;
//     }

//     // Check if it's an ELF file (verify the magic number)
//     if (memcmp(elf_header.e_ident, ELFMAG, SELFMAG) != 0) {
//         //printf("Not an ELF file\n");
//         fl_fclose(file);
//         return 1;
//     }

//     // Specify the entry point for the ELF program
//     void* entry_point = (void*)elf_header.e_entry;

//     // Allocate memory for the stack
//     void* stack = kmalloc(STACK_SIZE);
//     if (!stack) {
//         perror("Failed to allocate stack memory");
//         fl_fclose(file);
//         return 1;
//     }

//     // Set up the stack for the ELF program
//     setup_stack(stack + STACK_SIZE, entry_point);

//     // Close the ELF file
//     fl_fclose(file);

//     // Execute the ELF program (load it and start execution)

//     // Free the allocated stack memory when it's no longer needed
//     kfree(stack);

//     return 0;
// }

int execute_file(const char *path,int argc, char **argv)
{
    // global_file_path = path;
    LOG_LOCATION;
    for (size_t i = 0; i < argc; i++)
    {
        printf("%d:%s\n",i,argv[i]);
    }
    
    // internal_run_exe();
    lock_kb_input(127);
    LOG_LOCATION;
    load_elf_file(path,argc,argv);
    unlock_kb_input();
    

}

void internal_run_exe()
{
    //DISABLE context switching
    // LockAndPostpone();
    LOG_LOCATION;
    lock_kb_input(127);
    LOG_LOCATION;
    load_elf_file(global_file_path,global_argc,global_argv);
    unlock_kb_input();
    global_argc = 0;
    LOG_LOCATION;
    //ENABLE context switching
    // UnlockAndSchedule();
    // TerminateProcess();

}