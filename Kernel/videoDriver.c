#include <videoDriver.h>
#include <fonts.h>
#include <font_8x16.h>
#include <lib.h>

static Point cursorPos = {0 + X_MARGIN, 0 + Y_MARGIN};

struct vbe_mode_info_structure {
	uint16_t attributes;  // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;	  // deprecated
	uint8_t window_b;	  // deprecated
	uint16_t granularity; // deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr; // deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;		   // number of bytes per horizontal line
	uint16_t width;		   // width in pixels
	uint16_t height;	   // height in pixels
	uint8_t w_char;		   // unused...
	uint8_t y_char;		   // ...
	uint8_t planes;
	uint8_t bpp;   // bits per pixel in this mode
	uint8_t banks; // deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size; // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
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

	uint32_t framebuffer; // physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size; // size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed));

typedef struct vbe_mode_info_structure * VBEInfoPtr;

VBEInfoPtr VBE_mode_info = (VBEInfoPtr) 0x0000000000005C00;

const struct font_desc *default_font_desc = &font_vga_8x16;

static struct font_desc *desc;
unsigned int default_font_size;
static unsigned int font_size;

void initVideoDriver(){
	desc = (struct font_desc *)default_font_desc;
	default_font_size = (VBE_mode_info->height - 2 * Y_MARGIN) / (DEFAULT_LINES * (desc->height));
	font_size = default_font_size;
}

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y) {
	uint8_t * framebuffer = (uint8_t *) VBE_mode_info->framebuffer;
    uint64_t offset = (x * ((VBE_mode_info->bpp)/8)) + (y * VBE_mode_info->pitch);
    framebuffer[offset]     =  (hexColor) & 0xFF;
    framebuffer[offset+1]   =  (hexColor >> 8) & 0xFF; 
    framebuffer[offset+2]   =  (hexColor >> 16) & 0xFF;
}

void drawChar(Point topLeft, char c, uint32_t color, uint32_t bg_color){
	for(int i = 0; i < desc->height * font_size; i++){
		for(int j = 0; j < desc->width * font_size; j++){
			int condition = desc->data[c * desc->height + i / font_size];
			condition = condition & (1 << (desc->width - 1 - j / font_size));
			if(condition){
				putPixel(color, topLeft.x + j, topLeft.y + i);
			}else{
				putPixel(bg_color, topLeft.x + j, topLeft.y + i);
			}
		}
	}
	return;
}

uint64_t putChar(char c, uint32_t color, uint32_t bg_color){
	// font_name and unused_font_size are ignored now
	// desc and font_size are used instead

	// Optionally, you can still allow font_name to change desc/font_size if you want dynamic font switching
	// Otherwise, just ignore font_name and unused_font_size

	if(isValidY(cursorPos.y + desc->height * font_size) == 0){
		scrollUp(bg_color);
	}

	if(isValidX(cursorPos.x + desc->width * font_size) == 0){
		newline(bg_color);
	}

	if(c == '\n'){
		newline(bg_color);
		return 1;
	}
	if(c == '\b'){
		backspace(bg_color);
		return 1;
	}
	if(c == '\t'){
		for(int i = 0; i < 4; i++){
			putChar(' ', color, bg_color);
		}
		return 1;
	}
	else{
		drawChar(cursorPos, c, color, bg_color);
		cursorPos.x += desc->width * font_size;
	}

	return 1;
}

uint64_t putString(const char *string, uint32_t color, uint32_t bg_color){
	int i = 0;
	while(string[i] != 0){
		putChar(string[i], color, bg_color);
		i++;
	}
	return 1;
}

uint64_t putNString(const char *string, uint32_t color, uint32_t bg_color, uint64_t n){
	uint64_t res = 0;
	int i = 0;
	while(string[i] != 0 && i < n){
		res += putChar(string[i], color, bg_color);
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
	if(x > (VBE_mode_info->width - X_MARGIN) || x < X_MARGIN)
		return 0;
	return 1;
}

unsigned int isValidY(uint64_t y){
	if(y > (VBE_mode_info->height - Y_MARGIN) || y < Y_MARGIN)
		return 0;
	return 1;
}

void newline(uint32_t bg_color){
	resetCursor_x();
	cursorPos.y += desc->height * font_size;
	if(isValidY(cursorPos.y + desc->height * font_size) == 0){
		scrollUp(bg_color);
	}
}

void backspace(uint32_t bgc){
	if (isValidX(cursorPos.x - desc->width * font_size)) {
		cursorPos.x -= desc->width * font_size;
	} else {
		moveCursorEndPrevLine();
		if(isValidX(cursorPos.x - desc->width * font_size) == 0){
			resetCursor_x();
		}else{
			cursorPos.x -= desc->width * font_size;
		}
	}

	drawChar(cursorPos, ' ', bgc, bgc);
}

void clearScreen(uint32_t bg_color){
	for(int i = 0; i < VBE_mode_info->height; i++){
		for(int j = 0; j < VBE_mode_info->width; j++){
			putPixel(bg_color, j, i);
		}
	}
	resetCursor_x();
	resetCursor_y();
}

void scrollUp(uint32_t bg_color){
	uint8_t *framebuffer = (uint8_t*)VBE_mode_info->framebuffer;
	uint64_t row_size = VBE_mode_info->pitch * desc->height * font_size;
	uint64_t screen_size = VBE_mode_info->pitch * VBE_mode_info->height;

	memcpy(
		framebuffer,
		framebuffer + row_size,
		screen_size - row_size
	);

	// Clear the last row using memset for each pixel component
	uint64_t last_row_offset = screen_size - row_size;
	uint8_t r = (bg_color) & 0xFF;
	uint8_t g = (bg_color >> 8) & 0xFF;
	uint8_t b = (bg_color >> 16) & 0xFF;
	uint8_t bpp_bytes = VBE_mode_info->bpp / 8;

	for (uint64_t i = last_row_offset; i < screen_size; i += bpp_bytes) {
		framebuffer[i] = r;
		if (bpp_bytes > 1) framebuffer[i + 1] = g;
		if (bpp_bytes > 2) framebuffer[i + 2] = b;
	}

	// Move cursor to start of last row
	cursorPos.y -= desc->height * font_size;
	if (cursorPos.y < Y_MARGIN) {
		cursorPos.y = Y_MARGIN;
	}
}

uint64_t changeFontSize(uint64_t new_font_size){
	if(desc->height * new_font_size > VBE_mode_info->height - 2 * Y_MARGIN || desc->width * new_font_size > VBE_mode_info->width - 2 * X_MARGIN){
		return 0;
	}
	font_size = new_font_size;
	return 1;
}

moveCursorEndPrevLine(){
	cursorPos.x = (VBE_mode_info->width - X_MARGIN) - (VBE_mode_info->width - X_MARGIN * 2) % (desc->width * font_size);
	cursorPos.y -= desc->height * font_size;
}