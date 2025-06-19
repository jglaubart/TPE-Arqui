GLOBAL cpuVendor
GLOBAL getKeyPressed
GLOBAL outp
GLOBAL inp

EXTERN makeBackup
EXTERN getRipBackup

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

; Puede ir en otro archivo
getKeyPressed:
	push rbp
	mov rbp, rsp

    mov rax, 0

.cicle:
	in al, 64h
    and al, 0x01
    je .cicle
    in al, 60h

	mov rsp, rbp
	pop rbp
    ret

outp:
	mov rdx, rdi
	mov rax, rsi
	out dx, al
	ret

inp:
	mov rdx, rdi
	in al, dx
	ret

section .bss
    regsArr resb 144