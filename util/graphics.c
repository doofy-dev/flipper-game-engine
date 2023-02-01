#include "graphics.h"
#include <gui/canvas_i.h>
#include <math.h>
#include <u8g2_glue.h>
#include "dml.h"

bool in_screen(int16_t x, int16_t y) {
    return x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT;
}

bool test_pixel(const uint8_t *data, uint8_t x, uint8_t y, uint8_t w) {
    uint8_t current_bit = (y % 8);
    uint8_t current_row = ((y - current_bit) / 8);
    uint8_t current_value = data[current_row * w + x];

    return current_value & (1 << current_bit);
}

uint8_t *get_buffer(Canvas *const canvas) {
    return canvas->fb.tile_buf_ptr;
}

uint8_t *make_buffer(uint8_t w, uint8_t h) {
    return malloc(sizeof(uint8_t) * 8 * w * ceil(h / 8.0));
}

void clone_buffer(uint8_t *buf, uint8_t *data) {
    for (int i = 0; i < 1024; i++) {
        data[i] = buf[i];
    }
}

bool read_pixel(Canvas *const canvas, int16_t x, int16_t y) {
    if (in_screen(x, y)) {
        return test_pixel(get_buffer(canvas), x, y, SCREEN_WIDTH);
    }
    return false;
}

void draw_default_mode(Canvas *const canvas, const uint8_t *data, int16_t x, int16_t y, uint8_t w, uint8_t h) {
    for (int8_t _x = 0; _x <= w; _x++) {
        for (int8_t _y = 0; _y < h; _y++) {
            bool isOn = test_pixel(data, _x, _y, w);
            int8_t __x = _x + x;
            int8_t __y = _y + y;
            if(__x >= 0 && __y >= 0 && __y < SCREEN_HEIGHT)
                set_pixel(canvas->fb.tile_buf_ptr, __x, __y, SCREEN_WIDTH, isOn ? Black : White);
        }
    }
}

void draw_color_as(Canvas *const canvas, const uint8_t *data, int16_t x, int16_t y, uint8_t w, uint8_t h, bool is_black,
                   PixelColor draw_color) {
    for (int8_t _x = 0; _x <= w; _x++) {
        for (int8_t _y = 0; _y < h; _y++) {
            bool isOn = test_pixel(data, _x, _y, w);
            int8_t __x = _x + x;
            int8_t __y = _y + y;
            if (isOn == is_black && __x >= 0 && __y >= 0 && __y < SCREEN_HEIGHT)
                set_pixel(canvas->fb.tile_buf_ptr, __x, __y, SCREEN_WIDTH, draw_color);
        }
    }
}

void
draw_buffer(Canvas *const canvas, const uint8_t *data, int16_t x, int16_t y, uint8_t w, uint8_t h, DrawMode drawMode) {

    switch (drawMode) {
        case Default:
            draw_default_mode(canvas, data, x, y, w, h);
            break;
        case WhiteOnly:
            draw_color_as(canvas, data, x, y, w, h, false, White);
            break;
        case BlackOnly:
            draw_color_as(canvas, data, x, y, w, h, true, Black);
            break;
        case WhiteAsBlack:
            draw_color_as(canvas, data, x, y, w, h, false, Black);
            break;
        case BlackAsWhite:
            draw_color_as(canvas, data, x, y, w, h, true, White);
            break;
        case WhiteAsInverted:
            draw_color_as(canvas, data, x, y, w, h, false, Flip);
            break;
        case BlackAsInverted:
            draw_color_as(canvas, data, x, y, w, h, true, Flip);
            break;
    }
}

void set_pixel(uint8_t *buffer, int16_t x, int16_t y, uint8_t sw, PixelColor draw_mode) {
    uint8_t current_bit = (y % 8);
    uint8_t current_row = ((y - current_bit) / 8);
    uint32_t i = current_row * sw + x;
    uint8_t current_value = buffer[i];
    switch (draw_mode) {
        case Black:
            buffer[i] = current_value | (1 << current_bit);
            break;
        case White:
            buffer[i] = current_value & ~(1 << current_bit);
            break;
        case Flip:
            buffer[i] = current_value ^ (1 << current_bit);
            break;
    }
}