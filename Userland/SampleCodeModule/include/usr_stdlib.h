#ifndef _USRLIB_H_
#define _USRLIB_H_

#include <stdint.h>

#define PI 3.14159265358979323846

typedef enum
{
    STDIN = 0,
    STDOUT = 1,
    STDERR = 2,
    STDMARK = 3
} FileDescriptor;

uint64_t puts(const char *string);
uint64_t putChar(const char c);
uint64_t isChar(char c);
uint64_t isDigit(char c);
uint64_t strlen(const char *string);
uint64_t strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);

void unsigned_num_to_str(uint32_t num, uint32_t start, char *buff);
void signed_num_to_str(int32_t num, char *buff);
uint32_t unsigned_str_to_num(uint64_t *it, uint64_t buff_length, char *buff);
void unsigned_num_to_hex_str(uint32_t num, char *buff);
int32_t signed_str_to_num(uint64_t *it, uint64_t buff_length, char *buff);

uint8_t readChar(char *cp);
char getChar();
uint64_t readLine(char *buf, uint64_t size);
int changeFontSize(unsigned int newSize);


void InvalidOpCodeTest();
void zeroDivTest();
void getRegisters(); 
void sleep(uint64_t ticks);
void getTime();
uint64_t getScreenWidth();
uint64_t getScreenHeight();

double normalize_angle(double x);
double sin_taylor(double x);
double cos_taylor(double x);
#endif