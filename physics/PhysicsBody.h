#pragma once

#include "../types/Vector.h"
#include "../types/Component.h"
#include "Materials.h"

struct PhysicsMaterial;

class CircleCollider;

class PolyCollider;

class PhysicsBody : public Component<PhysicsBody> {
public:
    Vector gravity;
    bool is_fixed;
    float mass;
    float inertia;
    bool loaded = false;
    bool sleeping = false;
    PhysicsMaterial material;

    Vector velocity = {0, 0};
    Vector acceleration = {0, 0};
    Vector prev_position = {0, 0};

    PhysicsBody(const Vector &gravity, float mass, PhysicsMaterial m, bool fixed);

    void Process(const double &delta);

    void add_force(const Vector &force);

    void Start() override;

    void Destroy() override;

    bool is_loaded() { return loaded; }

    void wake_up() { sleeping = false; }

    void sleep() { sleeping = true; }
    bool is_sleep() { return sleeping; }

    void check_sleep();

    static void fix_position(CollisionInfo *info);

    static void resolve_bounce(CollisionInfo *info);

    static void resolve_friction(CollisionInfo *info, const float &time);
};
