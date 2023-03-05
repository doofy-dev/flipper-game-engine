#include "physics.h"
#include "collision.h"
#include "physics.h"
#include <furi.h>
#include <math.h>

void poly_compute(const Vector *points, int length, Vector *center, float *radius, Vector *result, const Matrix *m) {
    float r = 0;
    for (int i = 0; i < length; i++) {
        matrix_mul_vector(m, &(points[i]), &(result[i]));

        center->x += result[i].x;
        center->y += result[i].y;
        r = vector_magnitude(&(result[i]));
        if ((*radius) > r) {
            (*radius) = r;
        }
    }
    center->x /= (float) length;
    center->y /= (float) length;
}

Vector vertices[4];
void circle_poly_collision(PhysicsBody *c, PhysicsBody *p, CollisionInfo *result) {
    CollisionCircle *circle = (CollisionCircle *) c->collider;
    CollisionPolygon *polygon = (CollisionPolygon *) p->collider;
    result->collision = false;
    result->a = c;
    result->b = p;
    result->depth = INFINITY;

    Vector circleWorld, circleRadWorld, circleScale, polyCenter;
    //transform circle onto world space
    circleRadWorld = (Vector) {circle->radius, circle->radius};
    get_matrix_scale(&(c->transform->modelMatrix), &circleScale);
    vector_mul_components(&circleScale, &circleRadWorld, &circleRadWorld);
    get_matrix_translation(&(c->transform->modelMatrix), &circleWorld);
    float polyRadius;

    poly_compute(polygon->v, polygon->count, &polyCenter, &polyRadius, vertices, &(p->transform->modelMatrix));

    //test if it is close enough
    float dist = vector_distance(&polyCenter, &circleWorld);
    if (dist > (max(circleRadWorld.x, circleRadWorld.y) + polyRadius)) return;
    FURI_LOG_D("COLLIDE", "in range");
    Vector va, vb, projected;
    for (uint8_t i = 0; i < polygon->count; i++) {
        va = vertices[i];
        vb = vertices[(i + 1) % polygon->count];

        //project circle onto the line
        if (vector_project(&va, &vb, &circleWorld, &projected)) {
            float length = vector_magnitude(&projected);
            if (length > circleRadWorld.x) continue;

            result->depth = circleRadWorld.x - length;
            result->collision = true;
            vector_div_components(&circleWorld, &projected, &projected);
            vector_normalized(&projected, &(result->normal));
            return;
        }
    }
}

void poly_poly_collision(PhysicsBody *a, PhysicsBody *b, CollisionInfo *result) {
    result->collision = false;
    result->a = a;
    result->b = b;
    result->depth = 0;
    result->normal = VECTOR_ZERO;

    Vector aCenter = {0, 0}, bCenter = {0, 0};
    float aRadius = 0, bRadius = 0;
    CollisionPolygon *pca = (CollisionPolygon *) a->collider;
    CollisionPolygon *pcb = (CollisionPolygon *) b->collider;

    Vector polyA[MAX_POLY_SIZE];
    Vector polyB[MAX_POLY_SIZE];

    poly_compute(pca->v, pca->count, &aCenter, &aRadius, polyA, &(a->transform->modelMatrix));
    poly_compute(pcb->v, pcb->count, &bCenter, &bRadius, polyB, &(b->transform->modelMatrix));

    float dist = vector_distance(&aCenter, &bCenter);
    if (dist > (aRadius + bRadius)) return;

    Vector intersection, u, v, w, projected;
    Vector A1, A2, B1, B2;

    for (uint8_t i = 0; i < pca->count; i++) {
        A1 = polyA[i];
        A2 = polyA[(i + 1) % pca->count];

        for (uint8_t j = 0; j < pcb->count; j++) {
            B1 = polyB[j];
            B2 = polyB[(j + 1) % pcb->count];

            vector_sub(&A2, &A1, &u);
            vector_sub(&B2, &B1, &v);
            vector_sub(&A1, &B1, &w);
            float D = vector_cross(&u, &v);
            if (D == 0) continue;

            float t = vector_cross(&v, &w) / D;
            // The intersection is outside the line segment A
            if (t < 0 || t > 1) continue;

            float s = vector_cross(&u, &w) / D;
            // The intersection is outside the line segment B
            if (s < 0 || s > 1) continue;

            intersection.x = A1.x + t * u.x;
            intersection.y = A1.y + t * u.y;
            result->collision = true;

            // maybe allow multiple collisions? the current approach stops at one
            // and because of that it can take several cycles to resolve the overlaps
            result->normal = (Vector){u.y, -u.x};
            vector_normalized(&(result->normal), &(result->normal));
            projected.x = A1.x + result->normal.x * t * u.x;
            projected.y = A1.y + result->normal.y * t * u.y;
            result->depth = (float)sqrt(projected.x * projected.x + projected.y * projected.y);
            result->depth = vector_magnitude(&projected);
            vector_sub(&intersection, &bCenter, &w);
            if (vector_dot(&w, &result->normal) < 0) {
                // Make sure the normal points from body b to body a
                result->normal.x = -result->normal.x;
                result->normal.y = -result->normal.y;
            }
            //return on first collision
            return;
        }
    }
}
Vector ca,cb;
void circle_circle_collision(PhysicsBody *a, PhysicsBody *b, CollisionInfo *result) {
    CollisionCircle *collider_a = (CollisionCircle *) a->collider;
    CollisionCircle *collider_b = (CollisionCircle *) b->collider;
    result->collision = false;
    result->a = a;
    result->b = b;
    result->depth = 0;
    result->normal = VECTOR_ZERO;
    get_matrix_translation(&(a->transform->modelMatrix), &ca);
    get_matrix_translation(&(b->transform->modelMatrix), &cb);
    double dist = vector_distance(&ca, &cb);
    double radii = collider_a->radius + collider_b->radius;
    if (dist >= radii) return;

    result->collision = true;
    Vector normal;
    vector_sub(&ca, &cb, &normal);

    if (vector_magnitude(&normal) == 0) result->normal = (Vector){0, 1};
    else vector_normalized(&normal, &(result->normal));
    result->depth = radii - dist;
}


void collide(PhysicsBody *a, PhysicsBody *b, CollisionInfo *result) {
    if (a->type == CircleCollider && b->type == CircleCollider)
        circle_circle_collision(a, b, result);
    else if (a->type == CircleCollider && b->type == PolygonCollider)
        circle_poly_collision(a, b, result);
    else if (a->type == PolygonCollider && b->type == CircleCollider)
        circle_circle_collision(b, a, result);
    else if (a->type == PolygonCollider && b->type == PolygonCollider)
        poly_poly_collision(a, b, result);
}