#pragma once
#include <furi.h>
#include "dml.h"
#include <gui/canvas.h>
typedef struct Sprite sprite_t;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64


typedef enum {
    WhiteOnly,
    BlackOnly,
    WhiteAsBlack,
    BlackAsWhite,
    WhiteAsInverted,
    BlackAsInverted,
} DrawMode;

typedef enum {
    Black, //or
    White, //
    Flip   //not
} PixelColor;

typedef struct {
    uint8_t *data;
    const Icon *icon;
    bool loaded;
}ImageAsset;

//multiple sprite type implementation needed
struct Sprite{
    ImageAsset *asset;
    DrawMode draw_mode;
    Vector size;
    Vector anchor;
};

bool test_pixel(const uint8_t *data, uint8_t x, uint8_t y, uint8_t w);
bool read_pixel(Canvas *const canvas, int16_t x, int16_t y);
bool in_screen(int16_t x, int16_t y);
uint8_t* get_buffer(Canvas *const canvas);
uint8_t* make_buffer(uint8_t w, uint8_t h);
void clone_buffer(uint8_t* canvas, uint8_t* data);
void draw_buffer_scaled(uint8_t* canvas, sprite_t *const sprite, const Matrix* const m);
void set_pixel(uint8_t* canvas, int16_t x, int16_t y,  uint8_t w, PixelColor draw_mode);
sprite_t load_sprite(const Icon *icon);
void render_sprite(Canvas *const canvas,sprite_t *sprite);
ImageAsset *new_image_asset(const Icon *icon);
void clear_image_assets();
void clear_buffer(uint8_t* src);
void copy_to_screen_buffer(uint8_t* src, uint8_t* dst);
void draw_line(uint8_t *canvas, int x0, int y0, int x1, int y1, PixelColor mode);
void draw_circle(uint8_t *canvas, int x0, int y0, int r, PixelColor  mode);