#include "PolyCollider.h"
#include "CircleCollider.h"

PolyCollider::PolyCollider(Vector *_corners, uint8_t _count) : count(_count) {
    for (uint8_t i = 0; i < _count; i++){
        corners[i]=_corners[i];
    }
}

CollisionInfo PolyCollider::resolve(CircleCollider *other) {
    return CollisionInfo();
}

CollisionInfo PolyCollider::resolve(PolyCollider *other) {
    return CollisionInfo();
}
