#pragma once


#include "../types/Component.h"
#include "types.h"
#include "Collider.h"

#define MAX_POLY_SIZE 4

struct PolyComputeResult {
    Vector corners[4];
    float radius = 0;
    Vector center = {0, 0};
};

class CircleCollider;

class PolyCollider : public Collider<PolyCollider>{
    Vector corners[MAX_POLY_SIZE];
    uint8_t count;
public:
    PolyCollider(Vector *corners, uint8_t count);

    void Start() override;

    void Destroy() override;

    void compute_area_and_mass() override;

    CollisionInfo resolve(CircleCollider *other) override;

    CollisionInfo resolve(PolyCollider *other) override;

    PolyComputeResult compute();
};
