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


; Time and Date
global getSecs
global getMins
global getHours
global getDay
global getMonth
global getYear

getSecs:
    mov al, 0
    out 70h, al
    in al, 71h
    ret

getMins:
    mov al, 2
    out 70h, al
    in al, 71h
    ret

getHours:
    mov al, 4
    out 70h, al
    in al, 71h
    ret

getDay:
    mov al, 7
    out 70h, al
    in al, 71h
    ret

getMonth:
    mov al, 8
    out 70h, al
    in al, 71h
    ret

getYear:
    mov al, 9
    out 70h, al
    in al, 71h
    ret

;End Time and Date