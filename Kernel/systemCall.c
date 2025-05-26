#include "videoDriver.h"
#include "keyboard.h"
#include "lib.h"
#include <time.h>
#include "sound.h"

#define STDIN  0
#define STDOUT 1
#define STDERR 2

extern uint8_t hasInforeg;
extern const uint64_t inforeg[17];
extern int _hlt();
extern void capture_all_registers();

extern Color RED;
extern Color WHITE;
extern Color BLACK;
extern Color BLUE;

int size = 0;

// #define SYS_CALLS_NUM 14
#define SYS_CALLS_NUM 16

//llena buff con el caracter leido del teclado
static uint64_t sys_read(uint64_t fd, char * buff){
    if (fd != 0){
        return -1;
    }

    *buff = getCharFromKeyboard();
    return 0;
}


static uint64_t sys_write(uint64_t fd, char buffer) {
    if (fd != 1){
        return -1;
    }

    videoDriver_print(buffer,WHITE,BLACK);
    return 1;
}

static uint64_t sys_clear(){
    clearScreen(BLACK);
    return 1;
}

static uint64_t sys_getHours(){
    return getHours();
}

static uint64_t sys_getMinutes(){
    return getMinutes();
}

static uint64_t sys_getSeconds(){
    return getSeconds();
} 

static uint64_t sys_getScrHeight(){
    return getScreenHeight();
}

static uint64_t sys_getScrWidth(){
    return getScreenWidth();
}

static void sys_fillRect (int x, int y, int x2, int y2, Color color){
    videoDriver_fillRect (x,y,x2,y2,color);
}

static void sys_wait (int ms){
    if (ms > 0){
        int start_ms = ms_elapsed();
        do { _hlt(); } while (ms_elapsed() - start_ms < ms);
    }
}

//  Si capture == 1, captura los registros en inforeg.
//  Si capture == 0, devuelve el ultimo estado capturado en inforeg.
static uint64_t sys_inforeg(uint64_t registers[17], int capture){
    if(!capture && !hasInforeg) return hasInforeg;
    
    if(capture){
        capture_all_registers();
        return 1;
    }

    for(uint8_t i=0; i<17; i++){
        registers[i] = inforeg[i];
    }
    return 1;
}

static uint64_t sys_printmem ( uint64_t * address ){
    if((uint64_t) address > (0x20000000 - 32)){
      return -1;
    }

    uint8_t * aux = (uint8_t *) address;
        videoDriver_prints("\n",WHITE,BLACK);
    for(int i=0; i < 32 ; i++){
        videoDriver_printHex((uint64_t) aux, WHITE, BLACK);
        videoDriver_prints(" = ", WHITE, BLACK);
        videoDriver_printHex(*aux, WHITE, BLACK);
        videoDriver_newline();
        aux++;
    }

    return 0;
}


static uint64_t syscall_pixelUp(){
    increasePixelScale();
    sys_clear();
    return 1;
}

static uint64_t syscall_pixelDown(){
    decreasePixelScale();
    sys_clear();
    return 1;
}

static uint64_t sys_playSound(uint32_t freq){
    beep(freq);
    return 1;
}

static uint64_t sys_mute(){
    mute();
    return 1;
}



//por que sin el void tira warnings?
static uint64_t (*syscall_handlers[])(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8) = {
    (void*)sys_read, //0
    (void*)sys_write, //1
    (void*)sys_clear,  //2
    (void*)sys_getHours, //3
    (void*)sys_getMinutes, //4
    (void*)sys_getSeconds, //5
    (void*)sys_getScrHeight, //6
    (void*)sys_getScrWidth, //7
    (void*)sys_fillRect, //8
    (void*)sys_wait, //9
    (void*)sys_inforeg, //10
    (void*)sys_printmem, //11 
    (void*)syscall_pixelUp, //12
    (void*)syscall_pixelDown, //13
    (void*)sys_playSound, //14
    (void*)sys_mute //15
};


// Devuelve la syscall correspondiente
//                                rdi           rsi           rdx           rd10          r8           r9
uint64_t syscall_handler(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t rax) {
    if (rax < SYS_CALLS_NUM && syscall_handlers[rax] != 0){
        return syscall_handlers[rax](rdi, rsi, rdx, r10, r8);
    }
    return 0;
}



//la idea realmente fue hacerlo de esta forma con una sola funcion pero no pudimos hacer que funcione el sys_fillRect con el color, 
//ya que no nos permitia castear el r8 a tipo Color. Por eso veran un comentario en el case 8, explicando lo que queriamos hacer.

// uint64_t syscall_handler(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t rax) {
//     if (rax >= SYS_CALLS_QTY) {
//         return 0;
//     }

//     switch (rax) {
//         case 0:
//             return sys_read(rdi, (char *)rsi);
//         case 1:
//             return sys_write(rdi, (char)rsi);
//         case 2:
//             return sys_clear();
//         case 3:
//             return sys_getHours();
//         case 4:
//             return sys_getMinutes();
//         case 5:
//             return sys_getSeconds();
//         case 6:
//             return sys_getScrHeight();
//         case 7:
//             return sys_getScrWidth();
//         case 8:
//             // sys_fillRect((int)rdi, (int)rsi, (int)rdx, (int)r10, (Color)r8);
//             // return 0;
//         case 9:
//             sys_wait((int)rdi);
//             return 0;
//         case 10:
//             return sys_inforeg((uint64_t *)rdi);
//         case 11:
//             return sys_printmem((uint64_t *)rdi);
//         case 12:
//             return syscall_pixelUp();
//         case 13:
//             return syscall_pixelDown();
//         case 14:
//             return sys_playSound((uint32_t)rdi);
//         case 15:
//             return sys_mute();
//         default:
//             return 0;
//     }
// }


