#pragma once


#include "../types/Component.h"
#include "types.h"

class PolyCollider;
class CircleCollider : public Component<CircleCollider>, public Collider{
    float radius=1;
public:
    CircleCollider(float radius);
    CollisionInfo resolve(CircleCollider *other);

    CollisionInfo resolve(PolyCollider *other);
};
