#pragma once


#include "../types/Component.h"
#include "types.h"
#include "Collider.h"

class PolyCollider;

class CircleCollider : public Component<CircleCollider>, public Collider {
    float radius = 1;
public:
    explicit CircleCollider(float radius);

    void Start() override;

    void compute_area_and_mass() override;

    CollisionInfo resolve(CircleCollider *other);

    CollisionInfo resolve(PolyCollider *other);

    float get_radius() const { return radius; }

    void set_radius(float r) { radius = r; }

    void Destroy() override;
};