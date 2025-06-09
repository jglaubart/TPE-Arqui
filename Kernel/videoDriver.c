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

// Double-buffer pointers and size
static uint8_t *frontBuffer = 0;
static uint8_t *backBuffer  = 0;
static uint8_t *drawBuffer  = 0;
static uint64_t bufferSize  = 0;

const struct font_desc *default_font_desc = &font_vga_8x16;
static struct font_desc *desc;
unsigned int default_font_size;
static unsigned int font_size;

// Set which buffer to draw into: 0 = front, 1 = back
void setDrawBuffer(int buffer) {
    if (!frontBuffer) return;
    drawBuffer = (buffer == FRONT_BUFFER ? frontBuffer : backBuffer);
}

// Copy backBuffer into frontBuffer (display)
void showBackBuffer() {
    if (!frontBuffer || !backBuffer) return;
    memcpy(frontBuffer, backBuffer, bufferSize);
}

void initVideoDriver() {
    desc = (struct font_desc *)default_font_desc;
    default_font_size = (VBE_mode_info->height - 2 * Y_MARGIN)
                        / (DEFAULT_LINES * desc->height);
    font_size = default_font_size;

    // Initialize double buffer pointers
    frontBuffer = (uint8_t *)VBE_mode_info->framebuffer;
    bufferSize  = (uint64_t)VBE_mode_info->pitch * VBE_mode_info->height;
    backBuffer  = frontBuffer + bufferSize;

    drawBuffer  = frontBuffer; // Default to front buffer
}

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y) {
    if (!drawBuffer) return;
    uint64_t offset = x * ((VBE_mode_info->bpp) / 8)
                    + y * VBE_mode_info->pitch;
    drawBuffer[offset]   =  hexColor       & 0xFF;
    drawBuffer[offset+1] = (hexColor >> 8) & 0xFF;
    drawBuffer[offset+2] = (hexColor >> 16)& 0xFF;
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

void scrollUp(uint32_t bg_color) {
    if (!drawBuffer) return;
    uint64_t row_size    = VBE_mode_info->pitch * desc->height * font_size;
    uint64_t screen_size = bufferSize;
    // move up one text row
    memcpy(drawBuffer,
           drawBuffer + row_size,
           screen_size - row_size);
    // clear last row
    uint8_t r = bg_color & 0xFF;
    uint8_t g = (bg_color >> 8) & 0xFF;
    uint8_t b = (bg_color >> 16) & 0xFF;
    uint8_t bpp_bytes = VBE_mode_info->bpp / 8;
    for (uint64_t off = screen_size - row_size; off < screen_size; off += bpp_bytes) {
        drawBuffer[off] = r;
        if (bpp_bytes > 1) drawBuffer[off+1] = g;
        if (bpp_bytes > 2) drawBuffer[off+2] = b;
    }
    cursorPos.y -= desc->height * font_size;
    if (cursorPos.y < Y_MARGIN)
        cursorPos.y = Y_MARGIN;
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

uint64_t getScreenWidth() {
	return VBE_mode_info->width;
}

uint64_t getScreenHeight() {
	return VBE_mode_info->height;
}

//   Given an edge from p0 → p1, determines whether the horizontal scanline at
//   y = scanY intersects the edge (including the top endpoint, excluding the bottom).
//   If so, writes the intersection x into *outX and returns 1; otherwise returns 0.
//   All arithmetic uses signed intermediates to handle subtraction correctly.
int edgeIntersectsScanline(const Point *p0, const Point *p1, uint64_t scanY, int64_t *outX) {
    // Cast to signed for comparisons and arithmetic
    int64_t y0 = (int64_t)p0->y;
    int64_t y1 = (int64_t)p1->y;
    int64_t sy = (int64_t)scanY;

    // If edge is horizontal, there is no vertical span => no intersection
    if (y0 == y1) {
        return 0;
    }

    // Determine if scanY ∈ [min(y0,y1), max(y0,y1)) (top inclusive, bottom exclusive)
    int intersects = 0;
    if (y0 < y1) {
        if (sy >= y0 && sy < y1) {
            intersects = 1;
        }
    } else { // y1 < y0
        if (sy >= y1 && sy < y0) {
            intersects = 1;
        }
    }

    if (!intersects) {
        return 0;
    }

    // Compute intersection:
    //   x = p0.x + (scanY - p0.y) * (p1.x - p0.x) / (p1.y - p0.y)
    int64_t x0 = (int64_t)p0->x;
    int64_t x1 = (int64_t)p1->x;
    int64_t dx = x1 - x0;
    int64_t dy = y1 - y0;
    int64_t dy_scan = sy - y0;

    // Integer division; truncates toward zero.
    int64_t xi = x0 + (dx * dy_scan) / dy;
    *outX = xi;
    return 1;
}

//   Draws a filled circle of fixed radius, determined by the two opposing corners
//   corners[0] and corners[1], even if parts lie off-screen. Any pixel outside
//   the framebuffer is simply skipped, but the circle’s size and center remain
//   based solely on the original bounding box.
//
//   corners: array of two Points { x, y } defining opposite corners of the circle’s
//            axis-aligned bounding box.
//   color:   0x00RRGGBB color to fill the circle.
void drawCircle(const Point corners[2], uint32_t color) {
    // 1) Compute raw bounding box (no clamping yet)
    int64_t x0 = (int64_t)corners[0].x;
    int64_t y0 = (int64_t)corners[0].y;
    int64_t x1 = (int64_t)corners[1].x;
    int64_t y1 = (int64_t)corners[1].y;

    int64_t minX = (x0 < x1 ? x0 : x1);
    int64_t maxX = (x0 > x1 ? x0 : x1);
    int64_t minY = (y0 < y1 ? y0 : y1);
    int64_t maxY = (y0 > y1 ? y0 : y1);

    // 2) Compute circle center and radius from raw box
    int64_t boxW = maxX - minX;
    int64_t boxH = maxY - minY;
    if (boxW < 0) boxW = -boxW;
    if (boxH < 0) boxH = -boxH;
    int64_t centerX = minX + boxW / 2;
    int64_t centerY = minY + boxH / 2;
    int64_t r = (boxW < boxH ? boxW : boxH) / 2;
    if (r <= 0) {
        // Radius zero or negative -> draw nothing
        return;
    }
    int64_t r2 = r * r;

    // 3) Obtain screen dimensions
    uint64_t screenW = VBE_mode_info->width;
    uint64_t screenH = VBE_mode_info->height;

    // 4) Iterate y from (centerY - r) to (centerY + r), skipping off-screen scanlines
    int64_t y_start = centerY - r;
    int64_t y_end   = centerY + r;

    for (int64_t y = y_start; y <= y_end; y++) {
        if (y < 0 || (uint64_t)y >= screenH) {
            // Entire scanline is off-screen vertically; skip
        } else {
            int64_t dy = y - centerY;
            int64_t dy2 = dy * dy;
            int64_t rem = r2 - dy2;
            if (rem < 0) {
                // No horizontal span on this scanline
            } else {
                // 5) Find maxDx = floor(sqrt(rem)) without using sqrt()
                //    Since maxDx ≤ r, loop from 0 up to r
                int64_t maxDx = 0;
                int64_t test = 0;
                while (test <= r) {
                    if (test * test > rem) {
                        break;
                    }
                    maxDx = test;
                    test++;
                }

                // 6) Horizontal span runs from (centerX - maxDx) to (centerX + maxDx)
                int64_t x_start = centerX - maxDx;
                int64_t x_end   = centerX + maxDx;

                // 7) Draw each pixel in that span, skipping any off-screen horizontally
                for (int64_t x = x_start; x <= x_end; x++) {
                    if (x < 0 || (uint64_t)x >= screenW) {
                        // Skip off-screen pixel
                    } else {
                        putPixel(color, (uint64_t)x, (uint64_t)y);
                    }
                }
            }
        }
    }
}

//   Fills a convex quadrilateral defined by four Points (corners[0..3]).
//   Uses scanline polygon fill and clamps all pixels to screen bounds.
//   This version does not use `continue` or `break` inside loops.
void drawRectangle(const Point corners[4], uint32_t color){
    // 1) Find minY and maxY among the four corners (uint64_t)
    uint64_t minY = corners[0].y;
    uint64_t maxY = corners[0].y;
    for (int i = 1; i < 4; i++) {
        uint64_t yv = corners[i].y;
        if (yv < minY) {
            minY = yv;
        }
        if (yv > maxY) {
            maxY = yv;
        }
    }

    // 2) Clamp to vertical screen bounds
    uint64_t screenH = VBE_mode_info->height;
    if (minY >= screenH) {
        // Entire polygon is below the screen; nothing to draw
        return;
    }
    if (maxY >= screenH) {
        maxY = screenH - 1;
    }

    // 3) For each scanline y in [minY..maxY]:
    for (uint64_t y = minY; y <= maxY; y++) {
        int64_t xIntersects[4];
        int count = 0;

        // 3a) Compute intersections with each edge (corner[e] to corner[(e+1)%4])
        for (int e = 0; e < 4; e++) {
            const Point *p0 = &corners[e];
            const Point *p1 = &corners[(e + 1) % 4];
            int64_t xi;
            int intersected = edgeIntersectsScanline(p0, p1, y, &xi);
            if (intersected && count < 4) {
                xIntersects[count] = xi;
                count++;
            }
        }

        // 3b) Only proceed if we have at least two intersections
        if (count < 2) {
            // Fewer than 2 intersections -> no fill on this scanline
        } else {
            // 3c) Sort the intersection Xs (simple bubble sort for up to 4 items)
            for (int i = 0; i < count - 1; i++) {
                for (int j = i + 1; j < count; j++) {
                    if (xIntersects[j] < xIntersects[i]) {
                        int64_t tmp = xIntersects[i];
                        xIntersects[i] = xIntersects[j];
                        xIntersects[j] = tmp;
                    }
                }
            }

            // 3d) Fill between pairs: (xs[0]..xs[1]), then (xs[2]..xs[3]) if present
            uint64_t screenW = VBE_mode_info->width;
            for (int i = 0; i + 1 < count; i += 2) {
                int64_t startX = xIntersects[i];
                int64_t endX   = xIntersects[i + 1];

                // If the entire span is to the left of screen, skip via if:
                if (endX < 0) {
                    // skip this span
                } else {
                    // Clamp horizontal range to [0..screenW-1]
                    int64_t sx = startX;
                    int64_t ex = endX;
                    if (sx < 0) {
                        sx = 0;
                    }
                    if ((uint64_t)ex >= screenW) {
                        ex = screenW - 1;
                    }
                    // If, after clamping, sx <= ex, draw pixels
                    if (sx <= ex) {
                        for (int64_t x = sx; x <= ex; x++) {
                            // At this point x and y are within bounds; no need for extra check
                            putPixel(color, (uint64_t)x, y);
                        }
                    }
                }
            }
        }
    }
}