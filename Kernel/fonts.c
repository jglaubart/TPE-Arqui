#include <fonts.h>

static const struct font_desc *fonts[] = {
    &font_vga_8x8, &font_vga_8x16
};

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

const struct font_desc *find_font(const char *name)
{
	unsigned int i;

	int num_fonts = sizeof(fonts) / sizeof(fonts[0]);

	for (i = 0; i < num_fonts; i++)
		if (!strcmp(fonts[i]->name, name))
			return fonts[i];
	return 0;
}