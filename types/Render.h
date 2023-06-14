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

struct RenderInfo {
    Sprite *image = nullptr;
    Matrix matrix;
};

struct SpriteMap{
    Sprite *sprite = nullptr;
    Icon *icon = nullptr;
};