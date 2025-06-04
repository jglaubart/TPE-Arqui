#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <stdint.h>


enum{
    SYS_READ_ID = 0,
    SYS_WRITE_ID = 1,
    SYS_CLEAR_ID = 2,
    SYS_TIME_ID = 3,
    SYS_SLEEP_ID = 4,
    SYS_CHANGE_FONT_SIZE = 10,
    SYS_GET_REGS_ID = 11
};

typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} rtc_time_t;

extern uint64_t sys_call(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

#endif