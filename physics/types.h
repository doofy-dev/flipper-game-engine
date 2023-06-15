#pragma once

#include "../types/Vector.h"

class PhysicsBody;
struct CollisionInfo{
    PhysicsBody *a = nullptr;
    PhysicsBody *b = nullptr;
    Vector normal = {0,0};
    float depth = 0;
    bool is_collided = false;
};

struct PhysicsMaterial{
    float density;
    float bounciness;
    float static_friction;
    float dynamic_friction;
};

class Collider{};