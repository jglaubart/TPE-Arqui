#ifndef SOUND_H
#define SOUND_H
#include <stdint.h>

void playSound(uint64_t frequence);
void mute();
void beep(uint64_t freq, uint64_t ticks);

#endif