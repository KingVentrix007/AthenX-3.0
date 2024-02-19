





// Initializes the system, before calling kernel entry point kmain()
void init()
{

    kprints("");
    idt_init();
    
    // Initialize Global Descriptor Table
    gdt_init();
    
    // Print kernel start message
    kprints("Staring kernel\n");
}