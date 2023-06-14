#pragma once


#include "../types/Vector.h"
#include "../types/Component.h"
#include "Materials.h"

class PhysicsBody : public Component{
    Vector gravity;
    bool is_fixed;
    float mass;
    float inertia;
    PhysicsMaterial material;

    Vector velocity;
    Vector acceleration;
public:
    PhysicsBody(Vector gravity, float mass, PhysicsMaterial m, bool fixed);
    void Update(const float &delta) override;
};
