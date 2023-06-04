#pragma once


#include "Render.h"

class Sprite;

class Buffer {
    uint8_t _width, _height;
protected:
    uint8_t *data;
public:
    Buffer(uint8_t width, uint8_t height);

    bool test_pixel(uint8_t x, uint8_t y);

    void copy_into(uint8_t *other);

    void clear();

    bool test_coordinate(int x, int y) const;

    uint8_t width() { return _width; }
    uint8_t height() { return _height; }
};