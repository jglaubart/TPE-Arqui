#include <exceptions.h>
#include <stdint.h>
#include <videoDriver.h>
#include <syscall_handler.h>

#define REGISTERS 18

const char * registers[18] = {
    "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "RSP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15", "RIP", "RFLAGS"
};
static void zero_division();
static void invalid_opcode_exception();
static void convertToHex(uint64_t number, char buffer[16]);

void printRegisters(uint64_t *regs) {
    const char *registerTitles[REGISTERS] = {
        "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP",
        "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15", "RSP",
        "RIP", "RFLAGS"
    };

    // Buffer para convertir a hexadecimal
    char buffer[17];
    buffer[16] = '\0';

    // Imprimo los registros
    for (int i = 0; i < 18; i++) {
        putString(registerTitles[i], 0xFFFFFF);
        putString(": 0x", 0xFFFFFF);
        
        // Convert to hex without leading zeros
        if (regs[i] == 0) {
            putString("0", 0xFFFFFF);
        } else {
            convertToHex(regs[i], buffer);
            // Find first non-zero character
            int start = 0;
            while (start < 16 && buffer[start] == '0') {
                start++;
            }
            putString(buffer + start, 0xFFFFFF);
        }
        
        putString("\n", 0xFFFFFF);
    }
}
void exceptionDispatcher(int exception, const uint64_t regs[17]) {
	if (exception == ZERO_EXCEPTION_ID){
		zero_division();
	}
	else if(exception == INVALID_OPCODE_EXCEPTION_ID){
		invalid_opcode_exception();
	}
	printRegisters(regs);
	reset();
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

static void zero_division() {
	// Handler para manejar excepcíon
		putString("Zero division exception occurred.\n", 0xFFFFFF);
}

static void invalid_opcode_exception() {
	// Handler para manejar excepcíon
	putString("Invalid opcode exception occurred.\n", 0xFFFFFF);
}

