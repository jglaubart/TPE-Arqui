#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <stdint.h>
#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_EXCEPTION_ID 6

void exceptionDispatcher(int exception, const uint64_t regs[17]);
extern void reset();
#endif