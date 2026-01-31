#include "framebuffer.h"

#include <string.h>

#include "font.h"

uint8_t display_content[DISPLAY_PAGES][DISPLAY_WIDTH];

void framebuffer_clear(void) {
        memset(display_content, 0, (DISPLAY_WIDTH * DISPLAY_PAGES));
}

void framebuffer_print(const char *text, int row, int col, oled_color_t color) {
        col *= FONT_WIDTH;

        for(int i = 0; text[i] != '\0'; i++) {
                uint16_t char_index = (text[i] <= 0) ? 0 : text[i];
                for(uint8_t j = 0; j < FONT_WIDTH; j++){
                        if(color == OLED_COLOR_REGULAR) {
                                display_content[row][col] = FONTS[char_index][j];
                        }
                        else {
                                display_content[row][col] = ~FONTS[char_index][j];
                        }
                        col++;
                }
        }
}

void framebuffer_draw_pixel(int x, int y, oled_color_t color) {
        if(color == OLED_COLOR_REGULAR) {
                display_content[y/8][x] |= 1 << (y % 8);
        }
        else {
                display_content[y/8][x] &= ~(1 << (y % 8));
        }
}

void framebuffer_draw_horizontal_line(int width, int x, int y, oled_color_t color) {
        for(int i = x; i < x + width; i++) {
                framebuffer_draw_pixel(i, y, color);
        }
}

void framebuffer_draw_rectangle(int width, int height, int x, int y, oled_color_t color, bool fill) {
        for(int j = y; j < y + height; j++) {
                for(int i = x; i < x + width; i++) {
                        if((fill) || (j == y || j == y + height - 1 || i == x || i == x + width - 1)) {
                                framebuffer_draw_pixel(i, j, color); 
                        }
                }
        }
}
