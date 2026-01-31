#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>
#include <stdbool.h>

#include "resolution.h"

#define CENTRALIZE_ROW(h)   ((DISPLAY_ROWS - h)/2)
#define CENTRALIZE_COL(w)   ((DISPLAY_COLS - w)/2)

extern uint8_t display_content[DISPLAY_PAGES][DISPLAY_WIDTH];

typedef enum oled_color_t {
    OLED_COLOR_INVERTED,
    OLED_COLOR_REGULAR
} oled_color_t;

void framebuffer_clear(void);

void framebuffer_print(const char *text, int row, int col, oled_color_t color);

void framebuffer_draw_pixel(int x, int y, oled_color_t color);

void framebuffer_draw_horizontal_line(int width, int x, int y, oled_color_t color);

void framebuffer_draw_rectangle(int width, int height, int x, int y, oled_color_t color, bool fill);

#endif // FRAMEBUFFER_H
