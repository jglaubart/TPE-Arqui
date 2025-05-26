#ifndef VIDEODRIVER_H
#define VIDEODRIVER_H
#include <stdint.h>
// Function declarations and macros for the video driver

#define DIM_X 1024
#define DIM_Y 768

#define CHAR_WIDTH 8
#define CHAR_HEIGHT 16

typedef struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
} Color;

extern Color RED;
extern Color WHITE;
extern Color BLACK;
extern Color BLUE;


typedef struct {
	uint64_t x;
	uint64_t y;
}Point;
/**
 * @brief Puts a pixel on the screen at the specified coordinates with the given color.
 * 
 * @param hexColor The color of the pixel in hexadecimal format.
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 */
void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);

void printChar(Point topLeft, char c, uint32_t color, char *font_name, unsigned int font_size);

void printString(Point topLeft, char *string, uint32_t color, char *font_name, unsigned int font_size);

//////////////
void increasePixelScale();

// Disminuir el factor de escala para reducir el tamaño de un carácter
void decreasePixelScale();
// Obtener el ancho real de un carácter según el factor de escala actual
uint16_t getRealCharWidth();

// Obtener el alto real de un carácter según el factor de escala actual
uint16_t getRealCharHeight();

// Funciones para el manejo de texto en pantalla
void vdPrintString(const char *str, Color fnt, Color bgd);
void vdPrint(char c, Color fnt, Color bgd);
void vdNewline();
void vdBackspace(Color fnt, Color bgd);

// Limpia la pantalla con un color determinado de fondo
void clearScreen (Color color);

// Printea un value en los diversos formatos
void vdPrintDec(uint64_t value, Color fnt, Color bgd);
void vdPrintHex(uint64_t value, Color fnt, Color bgd);
void vdPrintBin(uint64_t value, Color fnt, Color bgd);
void vdPrintBase(uint64_t value, uint32_t base, Color fnt, Color bgd);

// Dibuja un rectangulo desde el punto topLeft
void vdFillRect (int x, int y, int x2, int y2, Color color);

// Getters
uint16_t getScreenWidth(void);
uint16_t getScreenHeight(void);
uint32_t getVideoFrameBuffer(void);
uint8_t getPixelWidth(void);
#endif

