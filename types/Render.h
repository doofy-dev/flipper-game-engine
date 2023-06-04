#pragma once

#include <gui/canvas.h>
#include <furi.h>
#include "Vector.h"
#include "Matrix.h"

class Sprite;

enum DrawMode {
    WhiteOnly,
    BlackOnly,
    WhiteAsBlack,
    BlackAsWhite,
    WhiteAsInverted,
    BlackAsInverted,
};

enum PixelColor {
    Black, //or
    White, //
    Flip   //not
};

struct ImageAsset {
    uint8_t *data;
    const Icon *icon;
    bool loaded;
};

struct RenderInfo {
    Sprite *image;
    Matrix matrix;
};
