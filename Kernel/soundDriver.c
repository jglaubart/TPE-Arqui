#include <stdint.h>
#include <rtc_time.h>
#include <soundDriver.h>
#include <lib.h>

void playSound(uint64_t frequence){
    uint64_t Div;
    uint8_t tmp;

    //Set the PIT to the desired frequency
    Div = 1193180 / frequence;
    outp(0x43, 0xb6);
    outp(0x42, (uint8_t) (Div) );
    outp(0x42, (uint8_t) (Div >> 8));

    //And play the sound using the PC speaker
    tmp = inp(0x61);
    if (tmp != (tmp | 3)) {
        outp(0x61, tmp | 3);
    }
}

void mute(){
    uint8_t tmp = inp(0x61) & 0xFC;
    outp(0x61, tmp);
}

void beep(uint64_t freq, uint64_t ms){
    playSound(freq);
    uint64_t ticks = msToTicks(ms);
    sleep(ticks);
    mute();
}