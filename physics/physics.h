#pragma once
#include "../util/dml.h"
#include "../objects.h"
#include "material.h"
#include <furi.h>
#define PHYSICS_UPDATE_MS 100

#ifndef INFINITY
#define INFINITY (1.0 / 0.0)
#endif

typedef struct PhysicsBody PhysicsBody;
typedef struct PhysicsSpace PhysicsSpace;
typedef struct PhysicsRegion PhysicsRegion;


struct PhysicsRegion {
    PhysicsBody **bodies;
    int count;
};

typedef struct {
    Vector v[4];
    uint8_t count;
} CollisionPolygon;

typedef struct {
    int radius;
} CollisionCircle;

typedef enum {
    PolygonCollider, CircleCollider
} PhysicsBodyType;

struct PhysicsBody{
    Vector gravity;
    bool fixed;

    PhysicsMaterial material;

    //calculated
    Vector velocity;
    Vector acceleration;
    //private
    PhysicsBodyType type;
    Vector world_pos;
    void *collider;
    transform_t *transform;
};

void process_physics_body(PhysicsBody *body, float time);

void physics_start();
void physics_stop();

void physics_clear();

PhysicsBody* new_physics_body(PhysicsMaterial m, bool fixed);
void add_physics_body(entity_t *entity, PhysicsBody *physicsBody);

void set_to_circle_collider(PhysicsBody *pb, Vector center, float radius);
void set_to_rectangle_collider(PhysicsBody *pb, Vector center, Vector size);
void set_to_polygon_collider(PhysicsBody *pb, Vector *corners, uint8_t count);