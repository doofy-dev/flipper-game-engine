#pragma once
#include "../objects.h"
#include "../util/dml.h"

typedef struct {
    Entity *a, *b;
    Vector normal;
    float depth;
    bool collision;
} CollisionInfo;

void collide(PhysicsBody *a, PhysicsBody *b, CollisionInfo *result);