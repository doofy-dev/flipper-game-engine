//
// Created by teeebor on 2023-06-04.
//

#include "RenderBuffer.h"
#include "Sprite.h"
#include <gui/canvas_i.h>
#include <u8g2_glue.h>

RenderBuffer::RenderBuffer(Canvas *c) : Buffer(canvas_width(c), canvas_height(c)), canvas(c) {

}


void RenderBuffer::render() {
//    canvas_reset(canvas);

    uint8_t *back = canvas->fb.tile_buf_ptr;
    canvas->fb.tile_buf_ptr = data;
    data = back;

    canvas_commit(canvas);
}

void RenderBuffer::reset() {
    render_count = 0;
}

void RenderBuffer::add_to_queue(Sprite *sprite, const Matrix &matrix) {
    render_list[render_count].image = sprite;
    render_list[render_count].matrix = matrix;
    render_count++;
}

void RenderBuffer::draw() {
    for (uint8_t i = 0; i < render_count; i++) {

    }
}

void RenderBuffer::set_pixel(int16_t x, int16_t y, PixelColor draw_mode) {
    uint8_t bit = 1 << (y & 7);
    uint8_t *p = data + (y >> 3) * width() + x;

    switch (draw_mode) {
        case Black:
            *p |= bit;
            break;
        case White:
            *p &= ~bit;
            break;
        case Flip:
            *p ^= bit;
            break;
    }
}

void RenderBuffer::draw_line(int x0, int y0, int x1, int y1, PixelColor draw_mode) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;

    while (true) {
        if (test_coordinate(x0, y0)) {
            set_pixel(x0, y0, draw_mode);
        }
        if (x0 == x1 && y0 == y1) break;
        int e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y0 += sy;
        }
    }
}

void RenderBuffer::draw_circle(int x, int y, int r, PixelColor color) {
    int16_t a = r;
    int16_t b = 0;
    int16_t decision = 1 - a;

    while (b <= a) {
        set_pixel_with_check(a + x, b + y, color);
        set_pixel_with_check(b + x, a + y, color);
        set_pixel_with_check(-a + x, b + y, color);
        set_pixel_with_check(-b + x, a + y, color);
        set_pixel_with_check(-a + x, -b + y, color);
        set_pixel_with_check(-b + x, -a + y, color);
        set_pixel_with_check(a + x, -b + y, color);
        set_pixel_with_check(b + x, -a + y, color);

        b++;
        if (decision <= 0) {
            decision += 2 * b + 1;
        } else {
            a--;
            decision += 2 * (b - a) + 1;
        }
    }
}


void RenderBuffer::set_pixel_with_check(int16_t x, int16_t y, PixelColor draw_mode) {
    if (test_pixel(x, y))
        set_pixel(x, y, draw_mode);
}

void RenderBuffer::draw_scaled(Sprite *const sprite, const Matrix *const m) {
    switch (sprite->draw_mode) {
        default:
        case BlackOnly:
            draw_sprite(sprite, true, Black, m);
            break;
        case WhiteOnly:
            draw_sprite(sprite, false, White, m);
            break;
        case WhiteAsBlack:
            draw_sprite(sprite, false, Black, m);
            break;
        case BlackAsWhite:
            draw_sprite(sprite, true, White, m);
            break;
        case WhiteAsInverted:
            draw_sprite(sprite, false, Flip, m);
            break;
        case BlackAsInverted:
            draw_sprite(sprite, true, Flip, m);
            break;
    }
}

void RenderBuffer::draw_sprite(Sprite *const sprite, bool is_black, PixelColor draw_color,
                               const Matrix *const m) {
    Vector scale = m->scale();
    Vector position = m->translation();
    Vector anchor;
    float rotation = m->rotation();

    int16_t new_w = round(sprite->width() * scale.x);
    int16_t new_h = round(sprite->height() * scale.y);

    int16_t offsetX = round(position.x - (sprite->anchor.x * (new_w / 2.0)));
    int16_t offsetY = round(position.y - (sprite->anchor.y * (new_h / 2.0)));

    float xInc = sprite->width() / (float) new_w;
    float yInc = sprite->height() / (float) new_h;

    uint16_t test_pixel_x;
    float pixel_x = 0;
    float pixel_y = 0;
    bool isOn;
    int16_t _x, _y;

    int16_t __x, __y;
    float cos_theta = cos(rotation - M_PI_2);
    float sin_theta = sin(rotation - M_PI_2);

    anchor.x = sprite->anchor.x * sprite->width();
    anchor.y = sprite->anchor.y * sprite->height();

    for (_x = 0; _x < new_w; _x++) {
        test_pixel_x = (int16_t) round(pixel_x);
        pixel_y = 0;
        for (_y = 0; _y < new_h; _y++) {
            // Perform rotation
            __x = _x - anchor.x;
            __y = _y - anchor.y;
            float rot_x = (__x) * cos_theta - (__y) * sin_theta;
            float rot_y = (__x) * sin_theta + (__y) * cos_theta;

            __x = (int16_t) round(rot_x + anchor.x + offsetX);
            __y = (int16_t) round(rot_y + anchor.y + offsetY);

            if (test_coordinate(__x, __y)) {
                isOn = sprite->test_pixel(test_pixel_x, (int16_t) round(pixel_y)) == is_black;

                if (isOn)
                    set_pixel(__x, __y, draw_color);
            }
            pixel_y += yInc;
        }
        pixel_x += xInc;
    }
}