#include "syscall_handler.h"
#include "videoDriver.h"
#include "keyboardDriver.h"
#include <rtc_time.h>
#include "lib.h"
#include <soundDriver.h>

#define REGISTERS 18
uint64_t syscall_handler(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9){
    uint64_t result;
    switch(rax){
        case(SYS_WRITE_ID):
            result = sys_write(rdi,(const char *) rsi, rdx);
            break;
        case(SYS_READ_ID):
            result = sys_read(rdi, (char *) rsi, rdx);
            break;
        case(SYS_CLEAR_ID):
            result = sys_clear();
            break;
        case(SYS_TIME_ID):
            result = sys_get_time(rdi);
            break;
        case(SYS_SLEEP_ID):
            result = sys_sleep(rdi);
            break;
        case(SYS_BEEP_ID):
            result = sys_beep(rdi, rsi);
            break;
        case(SYS_CHANGE_FONT_SIZE_ID):
            unsigned int new_size = rdi;
            result = changeFontSize(new_size);
            break;
        case(SYS_GET_REGS_ID):
            result = sys_get_regs(rdi);
            break;
        case(SYS_GET_SCREEN_WIDTH_ID):
            result = sys_get_screen_width();
            break;
        case(SYS_GET_SCREEN_HEIGHT_ID):
            result = sys_get_screen_height();
            break;
        case(SYS_DRAW_CIRCLE_ID):
            result = sys_draw_circle((int64_t (*)[2])rdi, rsi);
            break;
        case(SYS_DRAW_RECTANGLE_ID):
            result = sys_draw_rectangle((int64_t (*)[2])rdi, rsi);
            break;
        case(SYS_IS_PRESSED_ID):
            result = sys_is_pressed((char) rdi);
            break;
        case(SYS_SHOW_BACK_BUFFER_ID):
            result = sys_show_back_buffer();
            break;
        case(SYS_SET_DRAW_BUFFER_ID):
            result = sys_set_draw_buffer(rdi);
            break;
        case(SYS_CHANGE_BG_COLOR_ID):
            result = sys_change_bg_color((uint32_t)rdi);
            break;
        default:
            result = -1;
            break;
    }
    return result;
}

uint64_t sys_write(uint64_t fd, const char *buf, uint64_t count){
    uint32_t default_color;
    char *default_font = "VGA8x16";
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

    return putNString(buf, default_color, default_font);
}

uint64_t sys_read(uint64_t fd, char *buf, uint64_t count){
    unsigned char character;
    uint64_t i = 0;
    while (i < count && (character = nextFromBuffer()) != 0)
    {
        buf[i++] = character;
    }
    return i;
}

uint64_t sys_clear(){
    clearScreen();
    return 0;
}

uint64_t sys_beep(uint64_t freq, uint64_t ticks){
    beep(freq, ticks);
    return 0;
}

uint64_t sys_get_regs(uint64_t* toRetRegs){
    uint64_t backupDone = isBackupDone();
    if (backupDone) {
        uint64_t *regs = getRegs();
        for(int i = 0; i < REGISTERS; i++){
            toRetRegs[i] = regs[i];
        }
    }
    return backupDone;
}

uint64_t sys_get_time(uint64_t rdi){
    rtc_time_t * time = (rtc_time_t *) rdi;
    get_time(time);
    return time;
}

uint64_t sys_sleep(uint64_t ticksToWait){
    _sti();
    sleep(ticksToWait);
    _cli();
    return 0;
}

uint64_t sys_get_screen_width() {
    return getScreenWidth();
}
uint64_t sys_get_screen_height() {
    return getScreenHeight();
}

uint64_t sys_draw_circle(int64_t (*corners)[2], uint32_t color) {
    Point points[2];
    for (int i = 0; i < 2; i++) {
        points[i].x = corners[i][0];
        points[i].y = corners[i][1];
    }
    drawCircle(points, color);
    return 0;
}

uint64_t sys_draw_rectangle(int64_t (*corners)[2], uint32_t color) {
    Point points[4];
    for (int i = 0; i < 4; i++) {
        points[i].x = corners[i][0];
        points[i].y = corners[i][1];
    }
    drawRectangle(points, color);
    return 0;
}

uint64_t sys_is_pressed(char c) {
    return isCharPressed(c);
}

uint64_t sys_show_back_buffer(){
    showBackBuffer();
    return 0;
}

uint64_t sys_set_draw_buffer(int buffer) {
    setDrawBuffer(buffer);
    return 0;
}

uint64_t sys_change_bg_color(uint32_t color){
    changeBackgroundColor(color);
    return 0;
}