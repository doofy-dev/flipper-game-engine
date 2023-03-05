#include "physics.h"
#include "../util/util.h"
#include "collision.h"
#include "../util/list.h"
#include "solver.h"

Vector tmp;
List *physics_bodies;
bool physics_process = false;
FuriThread *physics_thread;

static int32_t physics_loop(void *ctx);

void process_physics_body(PhysicsBody *body, float time) {
    if (body->fixed) return;

    float half_tsq = (float) (0.5 * time * time);
    vector_mul(&(body->gravity), time, &tmp);
    vector_add(&tmp, &(body->acceleration), &(body->acceleration));
    float velocity_magnitude = vector_magnitude(&(body->velocity));
    Vector up=(Vector){0,1};
    float normal_force = vector_dot(&(body->gravity), &up);

    Vector friction_force = {0, 0};
    if (velocity_magnitude > 0) {
        float friction_coeff = body->material.dynamic_friction;
        float friction_magnitude = friction_coeff * normal_force;

        vector_normalized(&(body->velocity), &tmp);
        vector_mul(&tmp, -friction_magnitude, &friction_force);
    } else {
        float friction_coeff = body->material.static_friction;
        float friction_magnitude = friction_coeff * normal_force;

        vector_normalized(&(body->acceleration), &tmp);
        vector_mul(&tmp, -friction_magnitude, &friction_force);
    }
    vector_add(&(body->acceleration), &friction_force, &(body->acceleration));


    vector_mul(&(body->acceleration), time, &tmp);
    vector_add(&(body->velocity), &tmp, &(body->velocity));

    Vector new_pos = {
            body->transform->position.x + body->velocity.x * time + body->acceleration.x * half_tsq,
            body->transform->position.y + body->velocity.y * time + body->acceleration.y * half_tsq,
    };

    set_position(body->transform, new_pos);
    update_transform(body->transform);
}

void add_physics_body(entity_t *entity, PhysicsBody *physicsBody) {
    entity->physicsBody = physicsBody;
    physicsBody->transform = &(entity->transform);

    list_add(physics_bodies, physicsBody);
}

void physics_clear() {
    t_ListItem *item = physics_bodies->start;
    while (item) {
        void *collider = ((PhysicsBody *) item->data)->collider;
        if (collider)
            release(collider);

        item = item->next;
    }
    list_clear(physics_bodies);
}

PhysicsBody *new_physics_body(Vector gravity, float mass, PhysicsMaterial m, bool fixed) {
    PhysicsBody *result = allocate(sizeof(PhysicsBody));
    result->gravity = gravity;
    result->fixed = fixed;
    result->mass = mass;
    result->material = m;
    result->velocity = (Vector) {0, 0};
    result->acceleration = (Vector) {0, 0};
    return result;
}

void set_to_circle_collider(PhysicsBody *pb, float radius) {
    pb->type = CircleCollider;
    CollisionCircle *coll = (CollisionCircle *) allocate(sizeof(CollisionCircle));
    coll->radius = radius;
    pb->collider = coll;
}

void set_to_polygon_collider(PhysicsBody *pb, Vector *corners, uint8_t count) {
    pb->type = PolygonCollider;

    CollisionPolygon *coll = (CollisionPolygon *) allocate(sizeof(CollisionPolygon));
    coll->count = count;
    for (int i = 0; i < count; i++)
        coll->v[i] = corners[i];
    pb->collider = coll;
}

void physics_start() {
    FURI_LOG_D("FlipperGameEngine", "Starting physics thread");
    physics_bodies = make_list(sizeof(PhysicsBody));
    physics_thread = furi_thread_alloc_ex(
            "PhysicsThread", 3 * 1024, physics_loop, physics_bodies);
    if (physics_thread)
        furi_thread_start(physics_thread);
    else
        FURI_LOG_D("FlipperGameEngine", "Cannot start physics");
}

static int32_t physics_loop(void *ctx) {
    UNUSED(ctx);
    uint32_t last_time = 0;
    t_ListItem *item;
    item = physics_bodies->start;
    physics_process = true;
    PhysicsBody *body;
    FURI_LOG_I("FlipperGameEngine", "Computing area and mass");
    while (item) {
        if (item->data)
            compute_area_and_mass(item->data);
        item = item->next;
    }
    FURI_LOG_D("FlipperGameEngine", "Starting physics loop");
    while (physics_process) {
        uint32_t curr = furi_get_tick();
        uint32_t diff = curr - last_time;
        if (last_time > 0) {
            //update positions
            item = physics_bodies->start;
            while (item) {
                if (item->data) {
                    body = (PhysicsBody *) item->data;
                    process_physics_body(body, (float) diff / PHYSICS_UPDATE_MS);
                }
                item = item->next;
            }

            item = physics_bodies->start;
            CollisionInfo info;

            //Check and resolve collisions
            item = physics_bodies->start;
            while (item) {
                t_ListItem *item2 = item->next;
                while (item2) {
                    collide((PhysicsBody *) item->data, (PhysicsBody *) item2->data, &info);
                    if (info.collision) {
                        FURI_LOG_D("FlipperGameEngine", "%s collided with %s", info.a->transform->entity->name, info.b->transform->entity->name);

                        fix_position(&info);
                        resolve_bouncing(&info);
//                        resolve_friction(&info);
                    }
                    //resolve...

                    item2 = item2->next;
                }
                item = item->next;
            }
        }
        last_time = curr;
        furi_delay_ms(PHYSICS_UPDATE_MS);
    }
    FURI_LOG_D("FlipperGameEngine", "Stopping physics thread");
    return 0;
}

void physics_stop() {
    physics_process = false;
    furi_thread_join(physics_thread);
    furi_thread_free(physics_thread);
    physics_clear();
}


void compute_area_and_mass(PhysicsBody *body) {
    if (body->type == CircleCollider) {
        CollisionCircle *c = (CollisionCircle *) body->collider;
        float radSQ = c->radius * c->radius;
        body->mass = M_PI * radSQ * body->material.density;
        body->inertia = (float) (0.5 * body->mass * radSQ);
    } else if (body->type == PolygonCollider) {
        CollisionPolygon *p = (CollisionPolygon *) body->collider;
        body->mass = 0;
        float sum1 = 0, sum2 = 0, a, b;
        Vector v1, v2;
        int ip;
        for (int i = 0; i < p->count; i++) {
            ip = (i + 1) % p->count;
            v1 = p->v[i];
            v2 = p->v[ip];
            body->mass += vector_cross(&v1, &v2);
            a = vector_cross(&v2, &v1);
            b = vector_dot(&v1, &v1) + vector_dot(&v1, &v2) + vector_dot(&v2, &v2);
            sum1 += a * b;
            sum2 += a;
        }
        body->mass = (float) (0.5f * l_abs(body->mass) * body->material.density);
        body->inertia = (float) ((body->mass * sum1) / (6.0 * sum2));
    }
}

void add_force(PhysicsBody *body, Vector force) {
    vector_add(&(body->velocity), &force, &(body->velocity));
}
/*
void add_torque(PhysicsBody *body, float torque){
    body->torque+=torque;
}*/

#ifdef DRAW_COLLIDERS

List *get_colliders() {
    return physics_bodies;
}

#endif