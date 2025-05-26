#include <fonts.h>

static const struct font_desc *fonts[]{
    &font_vga_8x8, &font_vga_8x16
};

const struct font_desc *find_font(const char *name)
{
	unsigned int i;

	BUILD_BUG_ON(!num_fonts);
	for (i = 0; i < num_fonts; i++)
		if (!strcmp(fonts[i]->name, name))
			return fonts[i];
	return NULL;
}