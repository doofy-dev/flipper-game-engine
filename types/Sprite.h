#pragma once

#include "RenderBuffer.h"
#include "Render.h"
#include "Vector.h"

class Sprite : public Buffer {
    Icon *_icon;
    bool loaded;
    Vector anchor = {0.5, 0.5};
public:
    DrawMode draw_mode;

    Sprite(Icon *icon, DrawMode draw_mode);
    virtual ~Sprite() {
        LOG_D("Sprite cleared");
    }

    void render(Canvas *canvas);
    void set_anchor(float x, float y);
    Vector get_offset();

    bool is_loaded() const { return loaded; }
};