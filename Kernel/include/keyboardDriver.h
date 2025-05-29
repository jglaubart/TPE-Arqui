#ifndef KEYBOARDDRIVER_H
#define KEYBOARDDRIVER_H

extern void saveRegisters();
extern void getKeyPressed();

void writeInBuffer();
unsigned char nextFromBuffer();

#endif