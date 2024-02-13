ENTRY(main)

SECTIONS {
    . = 0x900000; /* Start address of program memory */
    
    .text : {
        *(.text)   /* .text sections (code) */
    }
    
    .data : {
        *(.data)   /* .data sections (initialized data) */
    }
    
    .bss : {
        *(.bss)    /* .bss sections (uninitialized data) */
    }
}
