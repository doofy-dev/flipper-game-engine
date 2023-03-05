#pragma once
#include "../util/dml.h"
#include "../objects.h"
#include "material.h"
#include "../config.h"

#include <furi.h>
#define PHYSICS_UPDATE_MS 10

#define MAX_POLY_SIZE 4

#ifndef INFINITY
#define INFINITY (1.0 / 0.0)
#endif

typedef struct PhysicsBody PhysicsBody;
typedef struct PhysicsSpace PhysicsSpace;
typedef struct CollisionCircle CollisionCircle;
typedef struct CollisionPolygon CollisionPolygon;

struct CollisionPolygon{
    Vector v[MAX_POLY_SIZE];
    uint8_t count;
};

struct CollisionCircle{
    float radius;
};

typedef enum {
    PolygonCollider, CircleCollider
} PhysicsBodyType;

struct PhysicsBody{
    Vector gravity;
    bool fixed;
    float mass;
    float inertia;

    PhysicsMaterial material;
//    float torque;
//    float angular_velocity;

    //calculated
    Vector velocity;
    Vector acceleration;

    //private
    PhysicsBodyType type;
    void *collider;
    transform_t *transform;
};

void process_physics_body(PhysicsBody *body, float time);

void physics_start();
void physics_stop();

void compute_area_and_mass(PhysicsBody *body);

void physics_clear();

PhysicsBody* new_physics_body(Vector gravity, float mass, PhysicsMaterial m, bool fixed);
void add_physics_body(entity_t *entity, PhysicsBody *physicsBody);
void add_force(PhysicsBody *body, Vector force);
//void add_torque(PhysicsBody *body, float torque);

void set_to_circle_collider(PhysicsBody *pb, float radius);
void set_to_polygon_collider(PhysicsBody *pb, Vector *corners, uint8_t count);

#ifdef DRAW_COLLIDERS
List* get_colliders();
#endif