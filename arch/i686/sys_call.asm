; ----------------------------------------------------------------
extern system_call_handler_c
global system_call_handler

system_call_handler:
    ; cli
    sti
    mov eax, [ebp + 8]    ; syscall_number
    mov ebx, [ebp + 12]   ; param1
    mov ecx, [ebp + 16]   ; param2

   
    push edx
    push esi
    push edi
    push ebp
    push edx
    push ecx
    push ebx
    
   
    push ecx
     push ebx
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax 
    mov gs, ax

    ; call handler
    ; mov edi, esp
    
    call system_call_handler_c

    ; restore registers
    pop ebx
    pop ecx
    pop edx
    pop ebp
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    ;pop eax
    add esp, 4 ; skip pushed eax - return result for system_call_handle_c
    ; cmp eax, 500
    ; je $

    sti
    iret
