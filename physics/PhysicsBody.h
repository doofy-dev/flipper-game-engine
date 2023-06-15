#pragma once

#include "../types/Vector.h"
#include "../types/Component.h"
#include "Materials.h"

struct PhysicsMaterial;
class PhysicsBody : public Component<PhysicsBody>{
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
