#include "syscall_handler.h"
#include "videoDriver.h"
#include "keyboardDriver.h"
#include <rtc_time.h>
#include "lib.h"
#include <time.h>

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
        case(SYS_CHANGE_FONT_SIZE_ID):
            unsigned int new_size = rdi;
            result = changeFontSize(new_size);
            break;
        case(SYS_GET_REGS_ID):
            result = sys_get_regs();
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

    return putNString(buf, default_color, default_bg_color, default_font);
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
    uint32_t bg_color = 0x000000; // Black background
    clearScreen(bg_color);
    return 0;
}

uint64_t sys_get_regs(){
    uint64_t *regs = getRegs();
 /*    for (int i = 0; i < REGISTERS; i++)
        arr[i] = regs[i]; */
    uint64_t backupDone = isBackupDone();
    if (backupDone) {
        printRegisters(regs);
    }
    return backupDone;
    
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
void printRegisters(uint64_t *regs) {
    const char *registerTitles[REGISTERS] = {
        "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "RSP",
        "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15",
        "RIP", "RFLAGS"
    };

    // Buffer para convertir a hexadecimal
char buffer[19];
    buffer[0] = '0';
    buffer[1] = 'x';
    buffer[18] = '\0';

	// Imprimo los registros
	for (int i = 0; i < 18; i++) {
        putString(registerTitles[i], 0xFFFFFF, 0x000000); //
        putString(" - ", 0xFFFFFF, 0x000000); 
        convertToHex(regs[i], buffer + 2);
        putString(buffer, 0xFFFFFF, 0x000000);
        if (i % 4 == 3)
            newline(0x000000);
        else
            putString(" || ", 0xFFFFFF, 0x000000); 
    }
    newline(0x000000);
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