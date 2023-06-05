#pragma once

#include "RenderBuffer.h"

class Sprite: public Buffer{
    Icon *icon;
    bool loaded;
public:
    DrawMode draw_mode;
    Vector anchor;
    Sprite(Icon *icon, DrawMode draw_mode);

    void render(Canvas *canvas);

    bool is_loaded() const{ return loaded;}
};
