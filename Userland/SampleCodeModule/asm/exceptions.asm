GLOBAL ex_invalid_opcode_exception
GLOBAL ex_zero_division_exception

section .text

ex_invalid_opcode_exception:
	ud2
	ret

ex_zero_division_exception:
	mov rax, 0
	div rax
	ret

section .bss