#pragma once

#include "../types/Vector.h"
#include "../types/Component.h"
#include "Materials.h"

struct PhysicsMaterial;

class PhysicsBody : public Component<PhysicsBody> {
public:
    Vector gravity;
    bool is_fixed;
    float mass;
    float inertia;
    PhysicsMaterial material;

    Vector velocity;
    Vector acceleration;

    PhysicsBody(const Vector& gravity, float mass, PhysicsMaterial m, bool fixed);

    void Update(const float &delta) override;

    void add_force(const Vector& force);

    void Start() override;

    void Destroy() override;

    void fix_position(CollisionInfo *info);
};
