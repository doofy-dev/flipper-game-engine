//
// Created by teeebor on 2023-06-04.
//

#include "Sprite.h"

Sprite::Sprite(Icon *icon, DrawMode d) : Buffer(icon_get_width(icon), icon_get_height(icon)) {
    draw_mode = d;
}
