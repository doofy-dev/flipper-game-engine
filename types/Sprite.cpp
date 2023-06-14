#include "Sprite.h"
#include "Buffer.h"
#include <gui/icon.h>

Sprite::Sprite(Icon *i, DrawMode d) : Buffer(i), _icon(i), draw_mode(d) {
    loaded = false;
}

void Sprite::render(Canvas *canvas) {
    LOG_D("Rendering sprite");

    RenderBuffer *buffer = new RenderBuffer(canvas);
    buffer->clear();
    buffer->swap();

    canvas_draw_icon(canvas, 0, 0, _icon);

    buffer->swap();

    for (uint8_t i = 0; i < width(); i++) {
        for (uint8_t j = 0; j <= height(); j++) {
            if (buffer->test_pixel(i, j)) {
                set_pixel(i, j, Black);
            }
        }
    }
    loaded = true;
    delete buffer;
}

void Sprite::set_anchor(float x, float y) {
    anchor.x=x;
    anchor.y=y;
}

Vector Sprite::get_offset() {
    return {
        anchor.x * (float)width(),
        anchor.y * (float)height()
    };
}
