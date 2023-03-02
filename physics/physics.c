#include "physics.h"
#include "util.h"
#include "collision.h"
#include "list.h"

Vector tmp;
List *physics_bodies;
bool physics_process = false;
FuriThread *physics_thread;

static int32_t physics_loop(void *ctx);

void process_physics_body(PhysicsBody *body, float time) {
    if (body->fixed) return;

    float half_tsq = 0.5 * time * time;
    vector_mul(&(body->gravity), time, &tmp);
    vector_add(&tmp, &(body->acceleration), &(body->acceleration));

    vector_mul(&(body->acceleration), time, &tmp);
    vector_add(&(body->velocity), &tmp, &(body->velocity));
    vector_mul(&(body->velocity), 1.0 - body->friction * time, &(body->velocity));

    Vector new_pos = {
            body->transform->position.x + body->velocity.x * time + body->acceleration.x * half_tsq,
            body->transform->position.y + body->velocity.y * time + body->acceleration.y * half_tsq,
    };

    set_position(body->transform, new_pos);
}

void add_physics_body(entity_t *entity, PhysicsBody *physicsBody) {
    if (physics_bodies == NULL)
        make_list(sizeof(PhysicsBody));

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

PhysicsBody *new_physics_body(float weight, Vector gravity, float friction, float bouncyness, bool fixed) {
    PhysicsBody *result = allocate(sizeof(PhysicsBody));
    result->weight = weight;
    result->gravity = gravity;
    result->friction = friction;
    result->bouncyness = bouncyness;
    result->fixed = fixed;
    result->velocity = (Vector) {0, 0};
    result->acceleration = (Vector) {0, 0};
    return result;
}

void add_circle_collider(PhysicsBody *pb, Vector center, float radius) {
    pb->type = CircleCollider;
    CollisionCircle *coll = (CollisionCircle *) allocate(sizeof(CollisionCircle));
    coll->center = center;
    coll->radius = radius;
    pb->collider = coll;
}

void add_rectangle_collider(PhysicsBody *pb, Vector center, Vector size) {
    pb->type = RectangleCollider;
    CollisionRectangle *coll = (CollisionRectangle *) allocate(sizeof(CollisionRectangle));
    coll->center = center;
    coll->size = size;
    pb->collider = coll;
}

void add_polygon_collider(PhysicsBody *pb, Vector *corners, uint8_t count) {
    pb->type = PolygonCollider;
    CollisionPolygon *coll = (CollisionPolygon *) allocate(sizeof(CollisionPolygon));
    coll->count = count;
    for (int i = 0; i < count; i++)
        coll->v[i] = corners[i];
    pb->collider = coll;
}

void physics_start() {
    FURI_LOG_D("FlipperGameEngine", "Starting physics thread");
    physics_thread = furi_thread_alloc_ex(
            "PhysicsThread", 30, physics_loop, physics_bodies);
    furi_thread_start(physics_thread);
}

static int32_t physics_loop(void *ctx) {
    UNUSED(ctx);
    uint32_t last_time = 0;

    FURI_LOG_D("FlipperGameEngine", "Starting physics loop");
    while (physics_process) {
        uint32_t curr = furi_get_tick();
        uint32_t diff = curr - last_time;

        //update positions
        t_ListItem *item = physics_bodies->start;
        PhysicsBody *body;
        while (item) {
            if (item->data) {
                body = (PhysicsBody *) item->data;
                process_physics_body(body, diff);
                get_matrix_translation(body->transform->modelMatrix, body->world_pos);
                body->world_pos
            }
            item = item->next;
        }

        item = physics_bodies->start;
        CollisionInfo info;
        //Check and resolve collisions
        while (item) {
            t_ListItem *item2 = item->next;
            while (item2) {
                collide((PhysicsBody *) item->data, (PhysicsBody *) item2->data, &info);
                //resolve...

                item2 = item2->next;
            }
            item = item->next;
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