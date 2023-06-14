#pragma once

#include "Buffer.h"
#include <gui/gui.h>


class RenderBuffer : public Buffer {
    Canvas *canvas;
    uint8_t render_count = 0;
    Buffer *front_buffer;
    RenderInfo render_list[32];
    bool ready=false;
    bool redraw= true;
    bool double_buffered=false;
    FuriMutex *mutex;

    void draw_sprite(Sprite *const sprite, bool is_black,
                     PixelColor draw_color, const Matrix &m);

public:
    RenderBuffer(Canvas *canvas);
    RenderBuffer(Canvas *canvas, bool doubleBuffer);

    virtual ~RenderBuffer();

    void render();

    void draw();

    void reset();

    void add_to_queue(Sprite *sprite, const Matrix &matrix);

    void draw_line(int x0, int y0, int x1, int y2, PixelColor draw_mode);

    void draw_circle(int x, int y, int r, PixelColor draw_mode);

    void draw_scaled(Sprite *const sprite, const Matrix &m);

    void swap();
    void swap(uint8_t *&buffer) override;
};
