#pragma once


#include "../types/Component.h"
#include "types.h"
#define MAX_POLY_SIZE 4

class CircleCollider;
class PolyCollider : public Component<PolyCollider>, public Collider{
    Vector corners[MAX_POLY_SIZE];
    uint8_t count;
public:
    PolyCollider(Vector *corners, uint8_t count);
    CollisionInfo resolve(CircleCollider *other);

    CollisionInfo resolve(PolyCollider *other);
};
