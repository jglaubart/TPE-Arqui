#include "usr_stdlib.h"
#include <syscalls.h>
#include <stdint.h>
#include <stdarg.h>

uint64_t puts(const char *string) {
    return sys_call(1, STDOUT, string, strlen(string), 0);
}

uint64_t strlen(const char *string) {
    uint64_t length = 0;
    while (string[length] != '\0') {
        length++;
    }
    return length;
}