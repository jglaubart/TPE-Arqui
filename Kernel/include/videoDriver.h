#ifndef _VIDEO_DRIVER_H_
#define _VIDEO_DRIVER_H_


#include <stdint.h>

typedef struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
} Color;

extern Color RED;
extern Color WHITE;
extern Color BLACK;
extern Color BLUE;


//////////////
void increasePixelScale();

// Disminuir el factor de escala para reducir el tamaño de un carácter
void decreasePixelScale();
// Obtener el ancho real de un carácter según el factor de escala actual
uint16_t getRealCharWidth();

// Obtener el alto real de un carácter según el factor de escala actual
uint16_t getRealCharHeight();
//////////////




/* Writes a string in screen*/
void videoDriver_prints(const char *str, Color fnt, Color bgd);

/* Writes a char in screen, given a font Color and Background Color*/
void videoDriver_print(char c, Color fnt, Color bgd);

/* Jumps to the next line */
void videoDriver_newline();

/* Erases the last char from the line, can't go up a line */
void videoDriver_backspace(Color fnt, Color bgd);

/* Clears screen with a given color */
void clearScreen (Color color);

/* prints in different bases */
void videoDriver_printDec(uint64_t value, Color fnt, Color bgd);
void videoDriver_printHex(uint64_t value, Color fnt, Color bgd);
void videoDriver_printBin(uint64_t value, Color fnt, Color bgd);
void videoDriver_printBase(uint64_t value, uint32_t base, Color fnt, Color bgd);

/* Given topLeft point draws a (color) rectangle */
void videoDriver_fillRect (int x, int y, int x2, int y2, Color color);

/* Plots a single pixel of a given color, if valid */
void videoDriver_setPixel(uint16_t x, uint16_t y, Color color);

/* Getters for screen */
uint16_t getScreenWidth(void);
uint16_t getScreenHeight(void);
uint32_t getVideoFrameBuffer(void);
uint8_t getPixelWidth(void);


#endif