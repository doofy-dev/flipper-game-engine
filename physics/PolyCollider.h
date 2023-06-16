#pragma once


#include "../types/Component.h"
#include "types.h"

#define MAX_POLY_SIZE 4

struct PolyComputeResult {
    Vector corners[4];
    float radius = 0;
    Vector center = {0, 0};
};

class CircleCollider;

class PolyCollider : public Component<PolyCollider> {
    Vector corners[MAX_POLY_SIZE];
    uint8_t count;
public:
    PolyCollider(Vector *corners, uint8_t count);

    CollisionInfo resolve(CircleCollider *other);

    CollisionInfo resolve(PolyCollider *other);

    PolyComputeResult compute();
};
