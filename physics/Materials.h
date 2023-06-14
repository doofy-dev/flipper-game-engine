#pragma once

typedef struct {
    float density;
    float bounciness;
    float static_friction;
    float dynamic_friction;
} PhysicsMaterial;


static const PhysicsMaterial Material_STEEL = {
        .density = 7.8f,
        .bounciness = 0.63f,
        .static_friction = 0.6f,
        .dynamic_friction = 0.4f
};

static const PhysicsMaterial Material_WOOD = {
        .density = 1.5f,
        .bounciness = 0.37f,
        .static_friction = 0.55f,
        .dynamic_friction = 0.3f
};

static const PhysicsMaterial Material_GLASS = {
        .density = 2.5f,
        .bounciness = 0.55f,
        .static_friction = 0.3f,
        .dynamic_friction = 0.19f
};

static const PhysicsMaterial Material_ICE = {
        .density = 0.92f,
        .bounciness = 0.05f,
        .static_friction = 0.07f,
        .dynamic_friction = 0.02f
};

static const PhysicsMaterial Material_CONCRETE = {
        .density = 3.6f,
        .bounciness = 0.2f,
        .static_friction = 0.89f,
        .dynamic_friction = 0.6f
};

static const PhysicsMaterial Material_RUBBER = {
        .density = 1.4f,
        .bounciness = 0.89f,
        .static_friction = 0.93f,
        .dynamic_friction = 0.75f
};
