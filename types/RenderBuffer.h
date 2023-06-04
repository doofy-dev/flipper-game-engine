#pragma once

#include "Buffer.h"
#include <gui/gui.h>


class RenderBuffer : public Buffer {
    Canvas *canvas;
    int render_count = 0;
    RenderInfo render_list[32];

    void draw_sprite(Sprite *const sprite, bool is_black,
                     PixelColor draw_color, const Matrix *const m);

public:
    RenderBuffer(Canvas *canvas);

    void render();

    void draw();

    void reset();

    void add_to_queue(Sprite *sprite, const Matrix &matrix);

    void draw_line(int x0, int y0, int x1, int y2, PixelColor draw_mode);

    void draw_circle(int x, int y, int r, PixelColor draw_mode);

    void draw_scaled(Sprite *const sprite, const Matrix *const m);

    void set_pixel(int16_t x, int16_t y, PixelColor draw_mode);

    void set_pixel_with_check(int16_t x, int16_t y, PixelColor draw_mode);

};
