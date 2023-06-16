//
// Created by teeebor on 2023-06-04.
//

#include "RenderBuffer.h"
#include "Sprite.h"
#include "../Helpers.h"
#include <gui/canvas_i.h>
#include <u8g2_glue.h>

RenderBuffer::RenderBuffer(Canvas *c) : Buffer(canvas_width(c), canvas_height(c)), canvas(c) {
    LOG_D("New renderBuffer");
    mutex = (FuriMutex *) furi_mutex_alloc(FuriMutexTypeNormal);

}

RenderBuffer::RenderBuffer(Canvas *c, bool doubleBuffer) : Buffer(canvas_width(c), canvas_height(c)), canvas(c) {
    LOG_D("New renderBuffer");
    if (doubleBuffer)
        front_buffer = new Buffer(canvas_width(c), canvas_height(c));
    double_buffered = doubleBuffer;
    mutex = (FuriMutex *) furi_mutex_alloc(FuriMutexTypeNormal);
}


void RenderBuffer::reset() {
    render_count = 0;
    redraw=true;
}

void RenderBuffer::add_to_queue(Sprite *sprite, const Matrix &matrix) {
    render_list[render_count].image = sprite;
    render_list[render_count].matrix = matrix;
    render_count++;
}

void RenderBuffer::swap() {
    uint8_t *back = canvas->fb.tile_buf_ptr;
    canvas->fb.tile_buf_ptr = data;
    data = back;
}

void RenderBuffer::swap(uint8_t *&buffer) {
    uint8_t *back = canvas->fb.tile_buf_ptr;
    canvas->fb.tile_buf_ptr = buffer;
    buffer = back;
}


void RenderBuffer::render() {
    furi_mutex_acquire(mutex, FuriWaitForever);
    if (double_buffered) {
        swap(front_buffer->data);
    } else {
        swap();
//        front_buffer->copy_from(canvas->fb.tile_buf_ptr);
    }
    furi_mutex_release(mutex);

    canvas_commit(canvas);
}

void RenderBuffer::draw() {
    ready = false;
    if(!redraw) return;
    redraw= false;

    if (!double_buffered)
        furi_mutex_acquire(mutex, FuriWaitForever);

    clear();
    for (uint8_t i = 0; i < render_count; i++) {
        RenderInfo info = render_list[i];
        check_pointer(info.image);
        if (info.image == NULL) {
            continue;
        }
        if (!info.image->is_loaded()) {
            info.image->render(canvas);
        }

        draw_scaled(info.image, info.matrix);
    }
    if (double_buffered) {
        furi_mutex_acquire(mutex, FuriWaitForever);
        front_buffer->swap(data);
    }
    furi_mutex_release(mutex);
    ready = true;
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

void RenderBuffer::draw_scaled(Sprite *const sprite, const Matrix &m) {
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
//TODO: proper scaling
#define newRender
#ifdef newRender
void RenderBuffer::draw_sprite(Sprite *const sprite, bool is_black, PixelColor draw_color, const Matrix &m) {
    Vector scale = m.scale();
    Vector position = m.translation();
    float rotation = m.rotation();
    Vector anchor = sprite->get_offset();
    float cosTheta = std::cos(rotation+M_PI_2);
    float sinTheta = std::sin(rotation+M_PI_2);

    for (int y = 0; y < sprite->height(); y++) {
        for (int x = 0; x < sprite->width(); x++) {
            float transformedX = (x - anchor.x) * scale.x;
            float transformedY = (y - anchor.y) * scale.y;
            float rotatedX = transformedX * cosTheta - transformedY * sinTheta;
            float rotatedY = transformedX * sinTheta + transformedY * cosTheta;

            int16_t finalX = (int16_t) round(rotatedX + position.x);
            int16_t finalY = (int16_t) round(rotatedY + position.y);
            if (test_coordinate(finalX, finalY)) {
                bool isOn = sprite->test_pixel(x, y) == is_black;
                if (isOn)
                    set_pixel(finalX, finalY, draw_color);
            }

        }
    }
}

#else
void RenderBuffer::draw_sprite(Sprite *const sprite, bool is_black, PixelColor draw_color, const Matrix &m) {
    Vector scale = m.scale();
    Vector position = m.translation();
    Vector anchor, coord;
    float rotation = m.rotation();

    Vector new_size{
            (float) round(sprite->width() * scale.x),
            (float) round(sprite->height() * scale.y)
    };

    Vector offset = position - (sprite->anchor * (new_size / 2.0));

    float xInc = sprite->width() / (float) new_size.x;
    float yInc = sprite->height() / (float) new_size.y;

    uint16_t test_pixel_x;
    Vector realCoord{0, 0};
    bool isOn;
    int16_t _x, _y;
    float theta = rotation - M_PI_2;

    anchor.x = sprite->anchor.x * sprite->width();
    anchor.y = sprite->anchor.y * sprite->height();

    for (_x = 0; _x < new_size.x; _x++) {
        test_pixel_x = (int16_t) round(realCoord.x);
        realCoord.y = 0;
        for (_y = 0; _y < new_size.y; _y++) {
            // Perform rotation
            coord.x = _x - anchor.x;
            coord.y = _y - anchor.y;
            coord.rotate(theta);
            coord = coord + anchor + offset;

            if (test_coordinate(coord.x, coord.y)) {
                isOn = sprite->test_pixel(test_pixel_x, (int16_t) round(realCoord.y)) == is_black;

                if (isOn)
                    set_pixel(coord.x, coord.y, draw_color);
            }
            realCoord.y += yInc;
        }
        realCoord.x += xInc;
    }
}
#endif

RenderBuffer::~RenderBuffer() {
    LOG_D("RenderBuffer end");

    check_pointer(front_buffer);
    if (double_buffered && front_buffer)
        delete front_buffer;
    furi_mutex_free(mutex);
}
