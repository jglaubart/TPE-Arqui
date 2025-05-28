#include "syscall_handler.h"
#include "videoDriver.h"
#include "lib.h"
#include <time.h>

uint64_t syscall_handler(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9){
    uint64_t result;
    switch(rax){
        case(WRITE_SYSCALL_ID):
            result = sys_write(rdi,(const char *) rsi, rdx);
            break;
        case(0):
            result = 0;
            break;
        default:
            result = -1;
            break;
    }
}

uint64_t sys_write(uint64_t fd, const char *buf, uint64_t count){
    uint32_t default_color;
    char *default_font = "VGA8x16";
    unsigned int default_font_size = 2;
    uint32_t default_bg_color = 0x000000;
    switch(fd){
        case(STDOUT):
            default_color = 0xFFFFFF;
            break;
        case(STDIN):
            default_color = 0xFFFFFF;
            break;
        case(STDERR):
            default_color = 0xFF0000;
            break;
        default:
            default_color = 0xFF0000;
            break;
    }

    return putNString(buf, default_color, default_bg_color, default_font, default_font_size, count);
}