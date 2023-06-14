#pragma once


#include "Render.h"

class Sprite;

class Buffer {
    uint8_t _width=0, _height=0;
public:
    uint8_t *data;
    Buffer(uint8_t width, uint8_t height);
    Buffer(Icon *icon);
    virtual ~Buffer();

    bool test_pixel(uint8_t x, uint8_t y);

    void copy_into(uint8_t *other);

    void clear();

    bool test_coordinate(int x, int y) const;
    void set_pixel(int16_t x, int16_t y, PixelColor draw_mode);
    void set_pixel_with_check(int16_t x, int16_t y, PixelColor draw_mode);
    void copy_from(uint8_t *other);
    uint8_t width() { return _width; }
    uint8_t height() { return _height; }
    virtual void swap(uint8_t *&buffer);
};