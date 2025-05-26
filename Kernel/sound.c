#include <stdint.h>
#include <time.h>
#include <sound.h>

void playSound(uint32_t frequence){
    uint32_t Div;
    uint8_t tmp;

    //Set the PIT to the desired frequency
    Div = 1193180 / frequence;
    outSpeaker(0x43, 0xb6);
    outSpeaker(0x42, (uint8_t) (Div) );
    outSpeaker(0x42, (uint8_t) (Div >> 8));

    //And play the sound using the PC speaker
    tmp = inSpeaker(0x61);
    if (tmp != (tmp | 3)) {
        outSpeaker(0x61, tmp | 3);
    }
}

void mute(){
    uint8_t tmp = inSpeaker(0x61) & 0xFC;
    outSpeaker(0x61, tmp);
}

void beep(uint32_t freq){
    playSound(freq);
    sleep(10);
    mute();
}