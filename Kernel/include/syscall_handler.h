#ifndef SYSCALL_HANDLER_H
#define SYSCALL_HANDLER_H

#include <stdint.h>

typedef enum{
    STDOUT = 1,
    STDIN = 0,
    STDERR = 2
} fd_t;

enum{
    SYS_READ_ID = 0,
    SYS_WRITE_ID = 1,
    SYS_CLEAR_ID = 2,
    SYS_CHANGE_FONT_SIZE_ID = 10
};


uint64_t syscall_handler(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9);

uint64_t sys_write(uint64_t fd, const char *buf, uint64_t count);
uint64_t sys_read(uint64_t fd, char *buf, uint64_t count);
uint64_t sys_clear();

#endif