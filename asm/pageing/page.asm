
global copy_page_physical:function copy_page_physical.end-copy_page_physical
copy_page_physical:
	push ebx				; __cdecl: we must preserve EBX.
	pushf					; push EFLAGS (to restore interrupts if necessary)
	cli						; disable interrupts

	mov ebx, [esp+12]		; Source address
	mov ecx, [esp+16]		; Destination address

	mov edx, cr0
	and edx, 0x7fffffff
	mov cr0, edx			; disable paging (we already got sourde and dest)

	mov edx, 1024			; 1024 * 4 bytes = 4096 bytes to copy

.loop:
	mov eax, [ebx]
	mov [ecx], eax			; copied
	add ebx, 4				; source address++
	add ecx, 4				; dest address++
	dec edx					; one less word
	jnz .loop

	mov edx, cr0
	or  edx, 0x80000000
	mov cr0, edx			; enable paging

	popf					; pop EFLAGS back
	pop ebx					; restore value of EBX
	ret
.end: