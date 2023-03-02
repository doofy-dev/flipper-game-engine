#pragma once

typedef struct {
    double density;
    double bouncyness;
    double static_friction;
    double dynamic_friction;
} PhysicsMaterial;


static const PhysicsMaterial Material_STEEL = {
        .density = 7.8,
        .bouncyness = 0.63,
        .static_friction = 0.6,
        .dynamic_friction = 0.4
};

static const PhysicsMaterial Material_WOOD = {
        .density = 1.5,
        .bouncyness = 0.37,
        .static_friction = 0.55,
        .dynamic_friction = 0.3
};

static const PhysicsMaterial Material_GLASS = {
        .density = 2.5,
        .bouncyness = 0.55,
        .static_friction = 0.3,
        .dynamic_friction = 0.19
};

static const PhysicsMaterial Material_ICE = {
        .density = 0.92,
        .bouncyness = 0.05,
        .static_friction = 0.07,
        .dynamic_friction = 0.02
};

static const PhysicsMaterial Material_CONCRETE = {
        .density = 3.6,
        .bouncyness = 0.2,
        .static_friction = 0.89,
        .dynamic_friction = 0.6
};

static const PhysicsMaterial Material_RUBBER = {
        .density = 1.4,
        .bouncyness = 0.89,
        .static_friction = 0.93,
        .dynamic_friction = 0.75
};
