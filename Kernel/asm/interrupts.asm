GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL interrupt_keyboard
GLOBAL interrupt_timerTick
GLOBAL exception_invalidOpCode
GLOBAL exception_divideByZero
GLOBAL interrupt_syscall

GLOBAL regdata_exc
GLOBAL inforeg
GLOBAL hasInforeg
GLOBAL capture_all_registers

EXTERN timer_handler
EXTERN keyboard_handler
EXTERN syscall_handler
EXTERN handle_exception
EXTERN videoDriver_newLine

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

%macro endOfHardwareInterrupt 0 ; para el keyboard y el timer tick
    ; signal pic EOI (End of Interrupt)
    mov al, 20h
    out 20h, al
%endmacro

%macro saveRegisters 0 ; para las excepciones
	mov [regdata_exc+0], 	rax ;0 
	mov [regdata_exc+8], 	rbx ;1
	mov [regdata_exc+16], 	rcx ;2
	mov [regdata_exc+24], 	rdx ;3
	mov [regdata_exc+32], 	rsi ;4
	mov [regdata_exc+40], 	rdi ;5
	mov [regdata_exc+48], 	rbp ;6
	;+ 56: RSP
	mov [regdata_exc+64], 	r8  ;8
	mov [regdata_exc+72], 	r9  ;9
	mov [regdata_exc+80], 	r10 ;10
	mov [regdata_exc+88], 	r11	;11
	mov [regdata_exc+96], 	r12 ;12
	mov [regdata_exc+104], 	r13 ;13
	mov [regdata_exc+112], 	r14 ;14
	mov [regdata_exc+120], 	r15 ;15
	;+ 128: RIP
	;+ 136: RFLAGS


	mov rax, rsp ; Obtenemos el valor de RSP cuando ocurrió la excepción sumando la cantidad de bytes pusheados al valor actual de RSP.
	add rax, 40 ; 40 bytes are pushed when an exception so we add 40 to RSP to get the value of RSP when the exception ocurred.
	mov [regdata_exc+56], rax

	mov rax, [rsp] ; Obtenemos el valor de RIP cuando ocurrió la excepción tomando la dirección de retorno de la interrupción.
	mov [regdata_exc+128], rax ;16

	mov rax, [rsp+8] ; We get the value of RFLAGS the same way (it is pushed when an interrupt occurs).
	mov [regdata_exc+136], rax ;17
%endmacro

_hlt: 
	sti
	hlt
	ret

_cli: ;clear interrupt
	cli
	ret


_sti: ;set interrupt
	sti
	ret

picMasterMask: ;mascara de interrupciones
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask: ;mascara de interrupciones
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn



capture_all_registers:
	mov [inforeg+2*8], rbx ;guardamos los registros
	mov [inforeg+3*8], rcx
	mov [inforeg+4*8], rdx
	mov [inforeg+5*8], rsi
	mov [inforeg+6*8], rdi
	mov [inforeg+7*8], rbp
	mov [inforeg+8*8], rsp
	mov [inforeg+9*8], r8
	mov [inforeg+10*8], r9
	mov [inforeg+11*8], r10
	mov [inforeg+12*8], r11
	mov [inforeg+13*8], r12
	mov [inforeg+14*8], r13
	mov [inforeg+15*8], r14
	mov [inforeg+16*8], r15

	mov rax, rsp
	add rax, 160 ;120 del popstate + 40 de la excepcion
	mov [inforeg+8*8], rax ;RSP (actualizado)

	mov rax, [rsp+15*8] 
	mov [inforeg], rax ;RIP
	
	mov rax, [rsp+14*8] ;RFLAGS
	mov [inforeg+1*8], rax ;
	

	mov byte [hasInforeg], 1 ;seteamos el flag ya que se hizo el dump de los registros (la funcion hasReg se fija si esta en 1)

	ret

interrupt_keyboard: ;interrupcion del teclado
	pushState ;guardamos los registros

	xor rax, rax
	in al, 60h 		; 60 es el puerto del teclado AL : 8 bits
	mov rdi, rax 	; recepcion del primer parametro

	cmp al, 0x1D 	;control izquierdo apretado
	jne .continue1
	mov byte [control], 1 ;seteamos el flag de shift izquierdo

.continue1:
	cmp al, 0x9D 	;control izquierdo liberado
	jne .continue2
	mov byte [control], 0

	;si estan apretados CONTROL + R se copian los registros en el vector regdataDump
	;mover RSP a donde estaba antes de ser llamada la excepcion
.continue2:
	cmp byte [control], 1 	; 'r' pressed
	jne .continue3
	cmp al, 0x13
	jne .continue3

	mov [inforeg+2*8], rbx ;guardamos los registros
	mov [inforeg+3*8], rcx
	mov [inforeg+4*8], rdx
	mov [inforeg+5*8], rsi
	mov [inforeg+6*8], rdi
	mov [inforeg+7*8], rbp
	mov [inforeg+8*8], rsp
	mov [inforeg+9*8], r8
	mov [inforeg+10*8], r9
	mov [inforeg+11*8], r10
	mov [inforeg+12*8], r11
	mov [inforeg+13*8], r12
	mov [inforeg+14*8], r13
	mov [inforeg+15*8], r14
	mov [inforeg+16*8], r15

	mov rax, rsp
	add rax, 160 ;120 del popstate + 40 de la excepcion
	mov [inforeg+8*8], rax ;RSP (actualizado)

	mov rax, [rsp+14*8] ;RFLAGS
	mov [inforeg+1*8], rax ;
	
	mov rax, [rsp+15*8] 
	mov [inforeg], rax ;RIP
	

	mov byte [hasInforeg], 1 ;seteamos el flag ya que se hizo el dump de los registros (la funcion hasReg se fija si esta en 1)

.continue3:
    call keyboard_handler

    endOfHardwareInterrupt
    popState
	iretq



interrupt_timerTick:
	pushState

	call timer_handler

	endOfHardwareInterrupt
	popState
	iretq

interrupt_syscall:
	mov rcx, r10
	mov r9, rax
	call syscall_handler
	iretq

haltcpu:
	cli
	hlt
	ret

exception_invalidOpCode:
	saveRegisters
	mov rdi, 06h
	mov rsi, regdata_exc 
	call handle_exception

exception_divideByZero:
	saveRegisters

	mov rdi, 00h
	mov rsi, regdata_exc 
	call handle_exception 

SECTION .bss
	aux resq 1
	regdata_exc		resq	18	; reservamos 18 bytes para guardar los registros para excepciones
	inforeg	resq	17	; reserve space for 17 qwords (one for each register we want to show on inforeg).
	hasInforeg 		resb 	1 	; reserve 1 byte for a boolean on whether a regdump has already occurred.
	control  	resb 	1   ; shift presionado