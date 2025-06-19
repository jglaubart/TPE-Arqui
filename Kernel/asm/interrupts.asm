GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler
GLOBAL _sys_int_handler

GLOBAL _exception00Handler
GLOBAL _exception06Handler

GLOBAL getRipBackup
GLOBAL saveRegisters

EXTERN irqDispatcher
EXTERN syscall_handler
EXTERN exceptionDispatcher
EXTERN getStackBase
EXTERN makeBackup

SECTION .text

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro

%macro saveRegistersMacro 2
	; After pushState, stack layout from top to bottom (RSP points to top):
	; r15 (RSP+0), r14 (RSP+8), r13 (RSP+16), r12 (RSP+24), r11 (RSP+32), r10 (RSP+40), r9 (RSP+48), r8 (RSP+56), rsi (RSP+64), rdi (RSP+72), rbp (RSP+80), rdx (RSP+88), rcx (RSP+96), rbx (RSP+104), rax (RSP+112)
	; Interrupt stack frame is at RSP+120: RIP, CS, RFLAGS, RSP, SS
	; We need to store in order: RAX, RBX, RCX, RDX, RSI, RDI, RBP, R8, R9, R10, R11, R12, R13, R14, R15, RSP, RIP, RFLAGS
	
	; RAX (stack offset 112)
	mov rdi, [%2 + 14*8]
	mov [%1], rdi
	; RBX (stack offset 104)
	mov rdi, [%2 + 13*8]
	mov [%1 + 1*8], rdi
	; RCX (stack offset 96)
	mov rdi, [%2 + 12*8]
	mov [%1 + 2*8], rdi
	; RDX (stack offset 88)
	mov rdi, [%2 + 11*8]
	mov [%1 + 3*8], rdi
	; RSI (stack offset 64)
	mov rdi, [%2 + 8*8]
	mov [%1 + 4*8], rdi
	; RDI (stack offset 72)
	mov rdi, [%2 + 9*8]
	mov [%1 + 5*8], rdi
	; RBP (stack offset 80)
	mov rdi, [%2 + 10*8]
	mov [%1 + 6*8], rdi
	; R8 (stack offset 56)
	mov rdi, [%2 + 7*8]
	mov [%1 + 7*8], rdi
	; R9 (stack offset 48)
	mov rdi, [%2 + 6*8]
	mov [%1 + 8*8], rdi
	; R10 (stack offset 40)
	mov rdi, [%2 + 5*8]
	mov [%1 + 9*8], rdi
	; R11 (stack offset 32)
	mov rdi, [%2 + 4*8]
	mov [%1 + 10*8], rdi
	; R12 (stack offset 24)
	mov rdi, [%2 + 3*8]
	mov [%1 + 11*8], rdi
	; R13 (stack offset 16)
	mov rdi, [%2 + 2*8]
	mov [%1 + 12*8], rdi
	; R14 (stack offset 8)
	mov rdi, [%2 + 1*8]
	mov [%1 + 13*8], rdi
	; R15 (stack offset 0)
	mov rdi, [%2]
	mov [%1 + 14*8], rdi
	; Original RSP (from interrupt stack frame)
	mov rdi, [%2 + 18*8]
	mov [%1 + 15*8], rdi
	; RIP (from interrupt stack frame)
	mov rdi, [%2 + 15*8]
	mov [%1 + 16*8], rdi
	; RFLAGS (from interrupt stack frame)
	mov rdi, [%2 + 17*8]
	mov [%1 + 17*8], rdi

%endmacro

%macro exceptionHandler 1
	pushState
	saveRegistersMacro regsExcArr, rsp
	call saveRegisters

	mov rdi, %1 ; pasaje de parametro
	mov rsi, regsExcArr
	call exceptionDispatcher

	popState

	call getStackBase

	mov [rsp+ 24], rax
	mov rax, userland
	mov [rsp],rax

	iretq

%endmacro


_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn


;8254 Timer (Timer Tick)
_irq00Handler:
	irqHandlerMaster 0

;Keyboard
_irq01Handler:
	; Save registers before calling irqDispatcher
	pushState
	saveRegistersMacro regsExcArr, rsp
	popState
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5

;System calls
_sys_int_handler:
	mov r9, r8
	mov r8, r10
	mov rcx, rdx
	mov rdx, rsi
	mov rsi, rdi
	mov rdi, rax

	call syscall_handler

	iretq

;Zero Division Exception
_exception00Handler:
	exceptionHandler 0

;Invalid Opcode Exception
_exception06Handler:
	exceptionHandler 6

haltcpu:
	cli
	hlt
	ret

saveRegisters:
	mov rdi, regsExcArr
	call makeBackup
	ret

section .rodata
	userland equ 0x400000

section .bss
	aux resq 1
	regsExcArr resb 144