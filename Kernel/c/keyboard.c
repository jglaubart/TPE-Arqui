#include "keyboard.h"
#include "time.h"
#include <stdint.h>

unsigned char keyCode = 0;
static char retChar = 0;
static int shift = 0 ;
static int capsLock = 0;



static const char hexMapPressed[256] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',//backspace,
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', //enter
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
        0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
        0, 0, 0, ' ', 0, 0 ,0,0,0,0,0,0,0, 0, 0, 0,
        0, 0 ,0,0,0,0,0,0,0,0,0,0,0
};



/*
 * 29: left cntrl
 * 42: left shift 0x2A
 * 54: right shift 0x36
 * 55: no tengo numpad
 * 56: left alt
 * 58: caps lock 0x3A
 * 59 - 68: F1-F103
 * 69: numLock
 * 70: scrollLock
 *
 * Up: 0x48
 * Left: 0x4B
 * Right: 0x4D
 * Down: 0x50
 */


void keyboard_handler(uint8_t keyPressed) { 
    keyCode = keyPressed;

    //capsLock
    if (keyCode == 0x3A) {
        capsLock = 1 - capsLock;
    }
    //shift pressed
    if (keyCode == 0x2A || keyCode == 0x36){
        shift = 1;
    }
    //shift not pressed
    if (keyCode == 0xAA || keyCode == 0xB6) {
        shift = 0;
    }
}


char getCharFromKeyboard() {
    if (keyCode > 0x80 || keyCode == 0x0F){ //release key
        retChar = 0;
    } else { //press key
        retChar = hexMapPressed[keyCode]; 
    }

    if ( (retChar >= 'a' && retChar <= 'z') && (shift == 1 || capsLock == 1) ){ //paso a mayus
        return retChar - ('a'-'A');
    }

    return retChar;
}

void clearKeyCode(){
    keyCode = 0;
}