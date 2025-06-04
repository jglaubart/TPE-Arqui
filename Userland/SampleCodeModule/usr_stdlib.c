#include "usr_stdlib.h"
#include <syscalls.h>
#include <stdarg.h>
#include <exceptions.h>

#define MAX_CHARS 1000
#define MAX_NUMBER_LENGTH 100

uint64_t puts(const char *string) {
    uint64_t count = strlen(string);
    return sys_call(SYS_WRITE_ID, STDOUT, string, count, 0);
}

uint64_t putChar(const char c){
    return sys_call(SYS_WRITE_ID, STDOUT, &c, 1, 0);
}


uint64_t isChar (char c){
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')){
		return 1;
	}
	return 0;
}

uint64_t isDigit(char c){
		if (c >= '0' && c <= '9'){
		return 1;
	}
	return 0;
}

uint64_t strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
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

static uint64_t fdprintfargs(FileDescriptor fd, const char *fmt, va_list args)
{
    char buffer[MAX_CHARS] = {0};

    char numstr[MAX_NUMBER_LENGTH] = {0};
    uint64_t i, j, k;
    for (i = 0, j = 0; j < MAX_CHARS && fmt[i] != 0; i++)
    {
        if (fmt[i + 1] != 0)
        {
            if (fmt[i] == '%')
            {
                switch (fmt[i + 1])
                {
                case 's':;
                    // Tengo que poner un string
                    const char *s = va_arg(args, const char *);
                    k = 0;
                    while (j < MAX_CHARS && s[k] != 0)
                        buffer[j++] = s[k++];
                    i++; // salteo la s
                    break;
                case 'd':;
                    // Tengo que poner un valor entero con signo
                    int32_t d = va_arg(args, int32_t);
                    signed_num_to_str(d, numstr);
                    k = 0;
                    while (j < MAX_CHARS && numstr[k] != 0)
                        buffer[j++] = numstr[k++];
                    i++; // salteo la d
                    break;
                case 'u':;
                    // Tengo que poner un valor entero sin signo
                    uint32_t u = va_arg(args, uint32_t);
                    unsigned_num_to_str(u, 0, numstr);
                    k = 0;
                    while (j < MAX_CHARS && numstr[k] != 0)
                        buffer[j++] = numstr[k++];
                    i++; // salteo la u
                    break;
                case 'c':;
                    // Tengo que poner un caracter
                    int8_t c = va_arg(args, int);
                    buffer[j++] = c;
                    i++; // salteo la c
                    break;
                case 'x':;
                    // Tengo que poner un valor entero en hexadecimal
                    uint32_t x = va_arg(args, uint32_t);
                    unsigned_num_to_hex_str(x, numstr);
                    k = 0;
                    while (j < MAX_CHARS && numstr[k] != 0)
                        buffer[j++] = numstr[k++];
                    i++; // salteo la x
                    break;
                default:
                    // Si no es ninguno pongo el porcentaje
                    buffer[j++] = '%';
                    break;
                }
            }
            else if (fmt[i] == '\\')
            {
                switch (fmt[i + 1])
                {
                case 'n':
                    buffer[j++] = '\n';
                    i++;
                    break;
                case 't':
                    buffer[j++] = '\t';
                    i++;
                    break;
                default:
                    buffer[j++] = '\\';
                    i++;
                    break;
                }
            }
            else
            {
                buffer[j++] = fmt[i];
            }
        }
        else
        {
            buffer[j++] = fmt[i];
        }
    }
    buffer[j++] = 0;
    return sys_call(SYS_WRITE_ID, fd, buffer, 1, 0);
}

uint64_t fdprintf(FileDescriptor fd, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    uint64_t toRet = fdprintfargs(fd, fmt, args);
    va_end(args);
    return toRet;
}

uint64_t printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    uint64_t toRet = fdprintfargs(STDOUT, fmt, args);
    va_end(args);
    return toRet;
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
    //putChar('\n');
    int res = sys_call(SYS_CHANGE_FONT_SIZE, newSize, 0, 0, 0);
    return res;
}

void InvalidOpCodeTest(){
    ex_invalid_opcode_exception();
}
void zeroDivTest(){
    ex_zero_division_exception();
}

void getRegisters() {
    sys_call(SYS_GET_REGS_ID, 0, 0, 0, 0);
}
void clearScreen() {
    sys_call(SYS_CLEAR_ID, 0, 0, 0, 0);
}

void sleep(uint64_t ticks) {
    sys_call(SYS_SLEEP_ID, ticks, 0, 0, 0);
}

void getTime() {
    rtc_time_t time = {0};
    sys_call(SYS_TIME_ID, &time, 0, 0, 0);
    printf("Time: %d:%d:%d   Date: %d/%d/%d\n", time.hours, time.minutes, time.seconds, time.day, time.month, time.year);
}