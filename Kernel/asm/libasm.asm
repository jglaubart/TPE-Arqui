GLOBAL cpuVendor

section .text
	
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

getSeconds:
	push rbp
	mov rbp, rsp

    mov al, 0x00
    out 70h, al
    in al, 71h

	mov rsp, rbp
	pop rbp
    ret

getMinutes:
	push rbp
	mov rbp, rsp

    mov al, 0x02
    out 70h, al
    in al, 71h

	mov rsp, rbp
	pop rbp
    ret

getHours:
	push rbp
	mov rbp, rsp

    mov al, 0x04
    out 70h, al
    in al, 71h

	mov rsp, rbp
	pop rbp
    ret

SECTION .bss
	aux resq 1
	regdata_exc		resq	18	; reservamos 18 bytes para guardar los registros para excepciones
	inforeg	resq	17	; reserve space for 17 qwords (one for each register we want to show on inforeg).
	hasInforeg 		resb 	1 	; reserve 1 byte for a boolean on whether a regdump has already occurred.
	control  	resb 	1   ; shift presionado