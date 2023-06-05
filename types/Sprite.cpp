//
// Created by teeebor on 2023-06-04.
//

#include "Sprite.h"

Sprite::Sprite(Icon *i, DrawMode d) : Buffer(icon_get_width(icon), icon_get_height(icon)) {
    draw_mode = d;
    loaded = false;
    icon = i;
}

void Sprite::render(Canvas *canvas) {
    RenderBuffer buffer(canvas);
    buffer.clear();
    buffer.swap();

    canvas_draw_icon(canvas, 0, 0, icon);

    buffer.swap();

    for (uint8_t i = 0; i < width(); i++) {
        for (uint8_t j = 0; j <= height(); j++) {
            if (buffer.test_pixel(i, j)) {
                set_pixel(i, j, Black);
            }
        }
    }
    loaded = true;
}
