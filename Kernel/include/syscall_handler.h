#ifndef SYSCALL_HANDLER_H
#define SYSCALL_HANDLER_H

#include <stdint.h>

typedef enum{
    STDOUT = 1,
    STDIN = 0,
    STDERR = 2
} fd_t;

#define WRITE_SYSCALL_ID 1
#define READ_SYSCALL_ID 0

uint64_t syscall_handler(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9);

uint64_t sys_write(uint64_t fd, const char *buf, uint64_t count);
uint64_t sys_read(uint64_t fd, char *buf, uint64_t count);

#endif