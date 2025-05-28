/*
 *  font.h -- `Soft' font definitions
 *
 *  Leve adaptacion del codigo de font.h de linux escrito en 1995 por Geert Uytterhoeven
 */

#ifndef _VIDEO_FONT_H
#define _VIDEO_FONT_H

struct font_desc {
    int idx;
    const char *name;
    unsigned int width, height;
    unsigned int charcount;
    const unsigned char *data;
    int pref;
};

#define VGA8x8_IDX	0
#define VGA8x16_IDX	1

extern const struct font_desc font_vga_8x8,
			font_vga_8x16;

/* Find a font with a specific name */

extern const struct font_desc *find_font(const char *name);

/* Get the default font for a specific screen size */

/* Max. length for the name of a predefined font */
#define MAX_FONT_NAME	32

struct font_data {
    const char *name;
	const unsigned char data[];
};

#endif /* _VIDEO_FONT_H */