#ifndef VIDEODRIVER_H
#define VIDEODRIVER_H
#include <stdint.h>
#include <fonts.h>
// Function declarations and macros for the video driver

#define DIM_X 1024
#define DIM_Y 768

#define X_MARGIN 10
#define Y_MARGIN 10

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

void drawChar(Point topLeft, char c, uint32_t color, uint32_t bg_color, const struct font_desc *desc, unsigned int font_size);

uint64_t putChar(char c, uint32_t color, uint32_t bg_color, char *font_name, unsigned int font_size);

uint64_t putString(const char *string, uint32_t color, uint32_t bg_color, char *font_name, unsigned int font_size);

uint64_t putNString(const char *string, uint32_t color, uint32_t bg_color, char *font_name, unsigned int font_size, uint64_t n);

unsigned int isValidX(uint64_t x);
unsigned int isValidY(uint64_t y);

void resetX();
void resetY();

void newline(const struct font_desc *desc, unsigned int font_size);

void backspace(const struct font_desc *desc, unsigned int font_size, uint32_t bgc);

#endif