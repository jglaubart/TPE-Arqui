#ifndef KEYBOARDDRIVER_H
#define KEYBOARDDRIVER_H

#include <stdint.h>
#include <lib.h>

extern void saveRegisters();
extern unsigned int getKeyPressed();

void writeInBuffer();
unsigned char nextFromBuffer();

uint8_t isMappedChar(uint8_t makeCode, int index);
uint8_t isAlpha(char c);

#endif