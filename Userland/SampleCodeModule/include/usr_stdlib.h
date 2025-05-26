#ifndef _USR_STDLIB_H_
#define _USR_STDLIB_H_

#include <stdint.h>
#include <colores.h>

/* Printea un char en pantalla */
void printChar (char c);

/* Printea un string en pantalla */
void printString (const char * str, int lenght);

/* Gets a single char from keyboard */
char getChar();

/*is c a char*/
int isChar(char c);

/*is digit*/
int isDigit(char c);

/* returns lenght of a string*/
int strlen(const char *str);

/* compares str1 with str2*/
int strcmp(const char *str1, const char *str2);

/*parses a num string into uint64_t*/
uint64_t charToInt(char* str);

/* prints in different bases */
void printDec(uint64_t value);
void printHex(uint64_t value);
void printBin(uint64_t value);
void printBase(uint64_t value, uint32_t base);


void clear_scr();
int getScreenWidth();
int getScreenHeight();
void paintRectangle(int x, int y, int x2, int y2, Color color);
void wait(uint64_t ms);
void inforeg(int capture);
void InvalidOpCodeTest();
void zeroDivTest();
//void sizePlus();
//void sizeMinus();
int print_mem(uint64_t mem);

void increaseScale();

void decreaseScale();

void captureRegisters();


#endif