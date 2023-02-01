#pragma once
#include <furi.h>
#include <gui/canvas.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64


typedef enum {
    Default,
    WhiteOnly,
    BlackOnly,
    WhiteAsBlack,
    BlackAsWhite,
    WhiteAsInverted,
    BlackAsInverted,
} DrawMode;

typedef enum {
    Black, White, Flip
} PixelColor;

bool test_pixel(const uint8_t *data, uint8_t x, uint8_t y, uint8_t w);
bool read_pixel(Canvas *const canvas, int16_t x, int16_t y);
bool in_screen(int16_t x, int16_t y);
uint8_t* get_buffer(Canvas *const canvas);
uint8_t* make_buffer(uint8_t w, uint8_t h);
void clone_buffer(uint8_t* canvas, uint8_t* data);
void draw_buffer(Canvas *const canvas, const uint8_t *data, int16_t x, int16_t y, uint8_t w, uint8_t h, DrawMode drawMode);
void set_pixel(uint8_t* canvas, int16_t x, int16_t y,  uint8_t w, PixelColor draw_mode);
