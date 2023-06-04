//
// Created by teeebor on 2023-06-04.
//

#pragma once


#include "Buffer.h"

class Sprite: public Buffer{
    ImageAsset *asset;
public:
    DrawMode draw_mode;
    Vector anchor;
    Sprite(Icon *icon, DrawMode draw_mode);
};
