#include "exceptions.h"
#include <stdint.h>
#include <videoDriver.h>

const char * registers[18] = {
    "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "RSP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15", "RIP", "RFLAGS"
};
static void zero_division();
static void invalid_opcode_exception();
static void convertToHex(uint64_t number, char buffer[16]);

void exceptionDispatcher(int exception, const uint64_t regs[17]) {
	if (exception == ZERO_EXCEPTION_ID)
		zero_division();
	else if(exception == INVALID_OPCODE_EXCEPTION_ID){
		invalid_opcode_exception();
	}

	char buffer[19];
    buffer[0] = '0';
    buffer[1] = 'x';
    buffer[18] = '\0';

	// Imprimo los registros
	for (int i = 0; i < 18; i++) {
        putString(registers[i], 0xFFFFFF, 0x000000); //
        putString(" - ", 0xFFFFFF, 0x000000); 
        convertToHex(regs[i], buffer + 2);
        putString(buffer, 0xFFFFFF, 0x000000);
        if (i % 4 == 3)
            newline(0x000000);
        else
            putString(" || ", 0xFFFFFF, 0x000000); 
    }

	reset();
}

static void zero_division() {
	// Handler para manejar excepcíon
		putString("Zero division exception occurred.\n", 0xFFFFFF, 0x000000);
}

static void invalid_opcode_exception() {
	// Handler para manejar excepcíon
	putString("Invalid opcode exception occurred.\n", 0xFFFFFF, 0x000000);
}

static void convertToHex(uint64_t number, char buffer[16]) {
    int index = 15;
    do {
        int remainder = number % 16;
        if (remainder < 10)
            buffer[index] = remainder + '0';
        else
            buffer[index] = remainder + 'A' - 10;
        number /= 16;
        index--;
    } while (index != -1);
}