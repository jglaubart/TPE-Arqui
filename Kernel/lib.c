#include <stdint.h>
#include <stdint.h>
#include "videoDriver.h"
#include "time.h"

#define ZERO_ID 0
#define INVAL_OPCODE_ID 6

extern void reset();
extern int _hlt(void);
static void convertToHex(uint64_t number, char buffer[16]);
void handle_exception(int ex, const uint64_t regs[17]);

Color red = {30,30,255};
Color white = {255,255,255};
Color black = {0,0,0};
Color blue = {255,0,0};

const char * registers[18] = {
    "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "RSP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15", "RIP", "RFLAGS "
};

void handle_exception(int ex, const uint64_t regs[17]) {
    char buffer[19];
    buffer[0] = '0';
    buffer[1] = 'x';
    buffer[18] = '\0';

    if (ex == ZERO_ID)
        videoDriver_prints("\nERROR! CPU EXCEPTION - Division by zero\n", white, red);
    else if (ex == INVAL_OPCODE_ID)
        videoDriver_prints("\nERROR! CPU EXCEPTION - Invalid opcode\n", white, red);

    for (int i = 0; i < 18; i++) {
        videoDriver_prints(registers[i], white, black);
        videoDriver_prints(" - ", white, black);
        convertToHex(regs[i], buffer + 2);
        videoDriver_prints(buffer, white, black);
        if (i % 4 == 3)
            videoDriver_newline();
        else
            videoDriver_prints(" || ", white, black);
    }

    reset();
}

void * memset(void * destination, int32_t c, uint64_t length)
{
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destination;

	while(length--)
		dst[length] = chr;

	return destination;
}

void * memcpy(void * destination, const void * source, uint64_t length)
{
	/*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
		(uint64_t)source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0)
	{
		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	}
	else
	{
		uint8_t * d = (uint8_t*)destination;
		const uint8_t * s = (const uint8_t*)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
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
