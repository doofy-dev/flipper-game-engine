#pragma once
#include "dml.h"
#include "../objects.h"
#include <furi.h>
#define PHYSICS_UPDATE_MS 100
typedef struct PhysicsBody PhysicsBody;

typedef struct {
    Vector v[4];
    uint8_t count;
} CollisionPolygon;

typedef struct {
    Vector center;
    Vector size;
} CollisionRectangle;

typedef struct {
    Vector center;
    int radius;
} CollisionCircle;

typedef enum {
    PolygonCollider, CircleCollider, RectangleCollider
} PhysicsBodyType;

typedef struct PhysicsBody{
    float weight;
    Vector gravity;
    float friction;
    float bouncyness;
    bool fixed;

    //calculated
    Vector velocity;
    Vector acceleration;
    //private
    PhysicsBodyType type;
    void *collider;
    transform_t *transform;
};

void process_physics_body(PhysicsBody *body, float time);

void check_collision(float time);

void physics_start();
void physics_stop();

void physics_clear();

PhysicsBody* make_physics_body(float weight, Vector gravity, float friction, float bouncyness, bool fixed);
void add_physics_body(entity_t *entity, PhysicsBody *physicsBody);

void add_circle_collider(PhysicsBody *pb, Vector center, float radius);
void add_rectangle_collider(PhysicsBody *pb, Vector center, Vector size);
void add_polygon_collider(PhysicsBody *pb, Vector *corners, uint8_t count);