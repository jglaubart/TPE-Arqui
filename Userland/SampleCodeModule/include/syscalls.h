#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <stdint.h>

enum {
    STDIN = 0,
    STDOUT = 1,
    STDERR = 2,
};

enum{
    SYS_WRITE_ID = 1
};

extern uint64_t sys_call(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

#endif