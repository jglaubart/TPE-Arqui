#include <videoDriver.h>
#include <fonts.h>
#include <font_8x16.h>

static Point cursorPos = {0 + X_MARGIN, 0 + Y_MARGIN};

struct vbe_mode_info_structure {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;
 
	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;
 
	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed));

typedef struct vbe_mode_info_structure * VBEInfoPtr;

VBEInfoPtr VBE_mode_info = (VBEInfoPtr) 0x0000000000005C00;

unsigned int isValidX(uint64_t x);
unsigned int isValidY(uint64_t y);

void resetX();
void resetY();

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y) {
    uint8_t * framebuffer = (uint8_t *) VBE_mode_info->framebuffer;
    uint64_t offset = (x * ((VBE_mode_info->bpp)/8)) + (y * VBE_mode_info->pitch);
    framebuffer[offset]     =  (hexColor) & 0xFF;
    framebuffer[offset+1]   =  (hexColor >> 8) & 0xFF; 
    framebuffer[offset+2]   =  (hexColor >> 16) & 0xFF;
}



void drawChar(Point topLeft, char c, uint32_t color, const struct font_desc *desc, unsigned int font_size){

	//put pixel para el char con el bitmap
	for(int i = 0; i < desc->height * font_size; i++){
		for(int j = 0; j < desc->width * font_size; j++){
			int condition = desc->data[c * desc->height + i / font_size];
			condition = condition & (1 << (desc->width - 1 - j / font_size));
			if(condition){
				putPixel(color, topLeft.x + j, topLeft.y + i);
			}
		}
	}

	return;
}

void printString(Point topLeft, char *string, uint32_t color, char *font_name, unsigned int font_size){
	const struct font_desc *desc = find_font(font_name);
	int i = 0;
	while(string[i] != 0){
		drawChar(topLeft, string[i], color, desc, font_size);
		topLeft.x += desc->width * font_size;
		i++;
	}
}

/* void printCharWorks(Point topLeft, char c, uint32_t color, unsigned int font_size){
	//put pixel para el char con el bitmap
	for(int i = 0; i < CHAR_HEIGHT * font_size; i++){
		for(int j = 0; j < CHAR_WIDTH * font_size; j++){
			int condition = fontdata_8x16[c * 16 + i / font_size];
			condition = condition & (1 << (CHAR_WIDTH - 1 - j / font_size));
			if(condition){
				putPixel(color, topLeft.x + j, topLeft.y + i);
			}
		}
	}

	return;
} */

uint64_t putChar(char c, uint32_t color, char *font_name, unsigned int font_size){

	const struct font_desc *desc = find_font(font_name);
	drawChar(cursorPos, c, color, desc, font_size);
	cursorPos.x += desc->width * font_size;

	if(isValidX(cursorPos.x + desc->width * font_size) == 0){
		resetCursor_x();
		cursorPos.y += desc->height * font_size;
		if(isValidY(cursorPos.y + desc->height * font_size) == 0){
			resetCursor_y();
		}
	}

	return 1;
}

uint64_t putString(const char *string, uint32_t color, char *font_name, unsigned int font_size){
	int i = 0;
	while(string[i] != 0){
		putChar(string[i], color, font_name, font_size);
		i++;
	}
	return 1;
}

uint64_t putNString(const char *string, uint32_t color, char *font_name, unsigned int font_size, uint64_t n){
	uint64_t res = 0;
	int i = 0;
	while(string[i] != 0 && i < n){
		res += putChar(string[i], color, font_name, font_size);
		i++;
	}
	return res;
}

void resetCursor_x(){
	cursorPos.x = 0 + X_MARGIN;
}

void resetCursor_y(){
	cursorPos.y = 0 + Y_MARGIN;
}

unsigned int isValidX(uint64_t x){
	if(x > (DIM_X - X_MARGIN) || x < X_MARGIN)
		return 0;
	return 1;
}

unsigned int isValidY(uint64_t y){
	if(y > (DIM_Y - Y_MARGIN) || y < Y_MARGIN)
		return 0;
	return 1;
}