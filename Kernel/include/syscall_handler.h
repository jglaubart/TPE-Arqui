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
    SYS_TIME_ID = 3,
    SYS_SLEEP_ID = 4,
    SYS_CHANGE_FONT_SIZE_ID = 10,
    SYS_GET_REGS_ID = 11,
    SYS_GET_SCREEN_WIDTH_ID = 12,
    SYS_GET_SCREEN_HEIGHT_ID = 13,
    SYS_DRAW_CIRCLE_ID = 14,
    SYS_DRAW_RECTANGLE_ID = 15,
    SYS_IS_PRESSED_ID = 16,
    SYS_SHOW_BACK_BUFFER_ID = 17,
    SYS_SET_DRAW_BUFFER_ID = 18
};


uint64_t syscall_handler(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9);

uint64_t sys_write(uint64_t fd, const char *buf, uint64_t count);
uint64_t sys_read(uint64_t fd, char *buf, uint64_t count);
uint64_t sys_clear();
uint64_t sys_get_regs();
uint64_t sys_get_time();
uint64_t sys_sleep(uint64_t ticksToWait);
uint64_t changeFontSize(uint64_t newSize);
uint64_t sys_get_screen_width();
uint64_t sys_get_screen_height();
uint64_t sys_draw_circle(int64_t (*corners)[2], uint32_t color);
uint64_t sys_draw_rectangle(int64_t (*corners)[2], uint32_t color);
uint64_t sys_is_pressed(char c);
uint64_t sys_show_back_buffer();
uint64_t sys_set_draw_buffer(int buffer);
void printRegisters(uint64_t *regs); 
#endif