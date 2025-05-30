#include "usr_stdlib.h"
#include <syscalls.h>
#include <stdarg.h>

#define MAX_NUMBER_LENGTH 100

uint64_t puts(const char *string) {
    uint64_t count = strlen(string);
    return sys_call(SYS_WRITE_ID, STDOUT, string, count, 0);
}

uint64_t putChar(const char c){
    return sys_call(SYS_WRITE_ID, STDOUT, &c, 1, 0);
}

uint64_t newLine(){
    return putChar('\n');
}


uint64_t strlen(const char *string) {
    uint64_t length = 0;
    while (string[length] != '\0') {
        length++;
    }
    return length;
}

char *strcpy(char *dest, const char *src) {
    uint64_t i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return dest;
}

//static
void unsigned_num_to_str(uint32_t num, uint32_t start, char *buff){
    uint32_t i = start;
    if (num == 0)
        buff[i++] = '0';
    while (i < MAX_NUMBER_LENGTH - 1 && num > 0)
    {
        buff[i++] = (num % 10) + '0';
        num /= 10;
    }
    buff[i] = 0;
    uint32_t revit = start;
    uint32_t revend = i - 1;
    while (revit < revend)
    {
        char aux = buff[revit];
        buff[revit] = buff[revend];
        buff[revend] = aux;
        revit++;
        revend--;
    }
}

//static
void signed_num_to_str(int32_t num, char *buff){
    uint32_t i = 0;
    if (num < 0)
    {
        buff[i++] = '-';
        num = -num;
    }
    unsigned_num_to_str(num, i, buff);
}

//static
uint32_t unsigned_str_to_num(uint64_t *it, uint64_t buff_length, char *buff){
    uint32_t num = 0;
    uint64_t i = *it;
    char debug[100];
    while (i < buff_length && buff[i] != ' ' && buff[i] != '\t')
    {
        num = num * 10 + (buff[i++] - '0');
    }
    *it = i;
    return num;
}

//static
void unsigned_num_to_hex_str(uint32_t num, char *buff){
    char hexDigits[] = "0123456789ABCDEF";
    uint32_t i = 0;
    if (num == 0)
        buff[i++] = '0';
    while (i < MAX_NUMBER_LENGTH - 1 && num > 0)
    {
        buff[i++] = hexDigits[num % 16];
        num /= 16;
    }
    buff[i] = 0;
    uint32_t revit = 0;
    uint32_t revend = i - 1;
    while (revit < revend)
    {
        char aux = buff[revit];
        buff[revit] = buff[revend];
        buff[revend] = aux;
        revit++;
        revend--;
    }
}

//static
int32_t signed_str_to_num(uint64_t *it, uint64_t buff_length, char *buff){
    int32_t mult = 1;
    if (buff[*it] == '-')
    {
        mult = -1;
        (*it)++;
    }
    return mult * unsigned_str_to_num(it, buff_length, buff);
}

uint8_t readChar(char *cp){
    return sys_call(SYS_READ_ID, STDIN, cp, 1, 0);
}

char getChar(){
    char c;
    uint64_t len;
    while((len = readChar(&c)) < 1);
    return c;
}

uint64_t readLine(char *buf, uint64_t size){
    uint64_t i = 0;
    char c;
    while((c = getChar()) != '\n'){
        if(i < size){
            buf[i] = c;
        }
        if(c == '\b' && i > 0){
            i += putChar(c);
            if(i < size){
                buf[i] = '\0';
            }
        }else{
            i += putChar(c);
        }
    }
    putChar('\n');
    uint64_t ans;
    if(i < size){
        buf[i] = '\0';
        ans = i;
    }else{
        buf[size - 1] = '\0';
        ans = size;
    }
    return ans;
}

int changeFontSize(unsigned int newSize){
    putChar('\n');
    return sys_call(SYS_CHANGE_FONT_SIZE, newSize, 0, 0, 0);
}