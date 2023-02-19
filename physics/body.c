/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#include <stdlib.h>
#include "./body.h"
#include "./array.h"
#include "./math.h"
#include "./aabb.h"
#include "./constants.h"
#include "./space.h"


/**
 * body.c
 * 
 * Body, body array and helper functions
 */


nv_Body *nv_Body_new(
    nv_BodyType type,
    nv_BodyShape shape,
    nv_Vector2 position,
    double angle,
    nv_Material material,
    double radius,
    nv_Array *vertices
) {
    nv_Body *body = (nv_Body *)malloc(sizeof(nv_Body));

    body->space = NULL;

    body->type = type;
    body->shape = shape;

    body->position = position;
    body->angle = angle;

    body->linear_velocity = nv_Vector2_zero;
    body->angular_velocity = 0.0;

    body->linear_damping = 0.0002;
    body->angular_damping = 0.0002;

    body->force = nv_Vector2_zero;
    body->torque = 0.0;

    body->material = material;

    body->is_sleeping = false;
    body->sleep_counter = 0;

    body->is_attractor = false;

    switch (shape) {
        case nv_BodyShape_CIRCLE:
            body->radius = radius;
            break;

        case nv_BodyShape_POLYGON:
            body->vertices = vertices;
            break;
    }

    nv_Body_calc_mass_and_inertia(body);

    return body;
}

void nv_Body_free(void *body) {
    if (body == NULL) return;
    nv_Body *b = (nv_Body *)body;

    switch (b->shape) {
        case nv_BodyShape_CIRCLE:
            b->radius = 0.0;
            break;

        case nv_BodyShape_POLYGON:
            nv_Array_free_each(b->vertices, free);
            nv_Array_free(b->vertices);
            b->vertices = NULL;
            break;
    }

    b->space = NULL;
    b->type = 0;
    b->shape = 0;
    b->position = nv_Vector2_zero;
    b->angle = 0.0;
    b->linear_velocity = nv_Vector2_zero;
    b->angular_velocity = 0.0;
    b->force = nv_Vector2_zero;
    b->torque = 0.0;
    b->is_sleeping = false;
    b->sleep_counter = 0;
    
    free(b);
}

void nv_Body_calc_mass_and_inertia(nv_Body *body) {
    double area;
    switch (body->shape) {
        case nv_BodyShape_CIRCLE:
            area = nv_circle_area(body->radius);
            body->mass = area * body->material.density;
            body->inertia = nv_circle_inertia(body->mass, body->radius);
            break;

        case nv_BodyShape_POLYGON:
            area = nv_polygon_area(body->vertices);
            body->mass = area * body->material.density;
            body->inertia = nv_polygon_inertia(body->mass, body->vertices);
            break;
    }

    switch (body->type) {
        case nv_BodyType_DYNAMIC:
            body->invmass = 1.0 / body->mass;
            body->invinertia = 1.0 / body->inertia;
            break;

        case nv_BodyType_STATIC:
            body->mass = 0.0;
            body->inertia = 0.0;
            body->invmass = 0.0;
            body->invinertia = 0.0;
            break;
    }
}

void nv_Body_integrate_accelerations(
    nv_Body *body,
    nv_Vector2 gravity,
    double dt
) {
    /*
        Integrate linear acceleration

        a = F * (1/M) + g
        v = a * Δt
    */
    nv_Vector2 linear_acceleration = nv_Vector2_add(
        nv_Vector2_muls(body->force, body->invmass), gravity);

    body->linear_velocity = nv_Vector2_add(
        body->linear_velocity, nv_Vector2_muls(linear_acceleration, dt));

    /*
        Integrate angular acceleration
        
        α = T * (1/I)
        ω = α * Δt
    */
    double angular_acceleration = body->torque * body->invinertia;
    body->angular_velocity += angular_acceleration * dt;
}

void nv_Body_integrate_velocities(nv_Body *body, double dt) {
    // Static bodies are static
    if (body->type == nv_BodyType_STATIC) {
        body->linear_velocity = nv_Vector2_zero;
        body->angular_velocity = 0.0;
        body->force = nv_Vector2_zero;
        body->torque = 0.0;
        return;
    }

    double ld = pow(0.98, body->linear_damping);
    double ad = pow(0.98, body->angular_damping);

    /*
        Integrate linear velocity

        v *= kᵥ (linear damping)
        x = v * Δt
    */
    body->linear_velocity = nv_Vector2_muls(body->linear_velocity, ld);
    body->position = nv_Vector2_add(body->position, nv_Vector2_muls(body->linear_velocity, dt));

    /*
        Integrate angular velocity

        ω *= kₐ (angular damping)
        θ = ω * Δt
    */
    body->angular_velocity *= ad;
    body->angle += body->angular_velocity * dt;

    // Reset forces
    body->force = nv_Vector2_zero;
    body->torque = 0.0;
}

void nv_Body_apply_attraction(nv_Body *body, nv_Body *attractor) {
    double distance = nv_Vector2_dist2(body->position, attractor->position);
    nv_Vector2 direction = nv_Vector2_sub(attractor->position, body->position);
    direction = nv_Vector2_normalize(direction);

    // Fg = (G * Mᴬ * Mᴮ) / d²
    double G = NV_GRAV_CONST * NV_GRAV_SCALE;
    double force_mag = (G * body->mass * attractor->mass) / distance;
    nv_Vector2 force = nv_Vector2_muls(direction, force_mag);

    nv_Body_apply_force(body, force);
}

void nv_Body_apply_force(nv_Body *body, nv_Vector2 force) {
    body->force = nv_Vector2_add(body->force, force);
}

void nv_Body_apply_force_at(
    nv_Body *body,
    nv_Vector2 force,
    nv_Vector2 position
) {
    body->force = nv_Vector2_add(body->force, force);
    body->torque += nv_Vector2_cross(position, force);
}

void nv_Body_sleep(nv_Body *body) {
    if (body->type != nv_BodyType_STATIC) {
        body->is_sleeping = true;
        body->linear_velocity = nv_Vector2_zero;
        body->angular_velocity = 0.0;
    }
}

void nv_Body_awake(nv_Body *body) {
    body->is_sleeping = false;
    body->sleep_counter = 0;
}

nv_AABB nv_Body_get_aabb(nv_Body *body) {
    switch (body->shape) {
        case nv_BodyShape_CIRCLE:
            return (nv_AABB){
                body->position.x - body->radius,
                body->position.y - body->radius,
                body->position.x + body->radius,
                body->position.y + body->radius
            };
            break;

        case nv_BodyShape_POLYGON:
            double min_x = NV_INF;
            double min_y = NV_INF;
            double max_x = -NV_INF;
            double max_y = -NV_INF;

            nv_Array *vertices = nv_Polygon_model_to_world(body);

            for (size_t i = 0; i < vertices->size; i++) {
                double vx = ((nv_Vector2 *)vertices->data[i])->x;
                double vy = ((nv_Vector2 *)vertices->data[i])->y;
                if (vx < min_x) min_x = vx;
                if (vx > max_x) max_x = vx;
                if (vy < min_y) min_y = vy;
                if (vy > max_y) max_y = vy;
            }

            nv_Array_free_each(vertices, free);
            nv_Array_free(vertices);

            return (nv_AABB){min_x, min_y, max_x, max_y};
            break;
    }
}

double nv_Body_get_kinetic_energy(nv_Body *body) {
    // 1/2 * M * v²
    return 0.5 * body->mass * nv_Vector2_len2(body->linear_velocity);
}

double nv_Body_get_rotational_energy(nv_Body *body) {
    // 1/2 * I * ω²
    return 0.5 * body->inertia * fabs(body->angular_velocity);
}

void nv_Body_set_is_attractor(nv_Body *body, bool is_attractor) {
    if (body->is_attractor != is_attractor) {
        body->is_attractor = is_attractor;
        
        if (body->is_attractor) {
            nv_Array_add(body->space->attractors, body);
        }
        else {
            nv_Array_remove(body->space->attractors, body);
        }
    }
}

bool nv_Body_get_is_attractor(nv_Body *body) {
    return body->is_attractor;
}


nv_Body *nv_Circle_new(
    nv_BodyType type,
    nv_Vector2 position,
    double angle,
    nv_Material material,
    double radius
) {
    return nv_Body_new(
        type,
        nv_BodyShape_CIRCLE,
        position,
        angle,
        material,
        radius,
        NULL
    );
}

nv_Body *nv_Polygon_new(
    nv_BodyType type,
    nv_Vector2 position,
    double angle,
    nv_Material material,
    nv_Array *vertices
) {
    return nv_Body_new(
        type,
        nv_BodyShape_POLYGON,
        position,
        angle,
        material,
        0.0,
        vertices
    );
}

nv_Body *nv_Rect_new(
    nv_BodyType type,
    nv_Vector2 position,
    double angle,
    nv_Material material,
    double width,
    double height
) {
    double w = width / 2.0;
    double h = height / 2.0;

    nv_Array *vertices = nv_Array_new();
    nv_Array_add(vertices, nv_Vector2_heap(-w, -h));
    nv_Array_add(vertices, nv_Vector2_heap(w, -h));
    nv_Array_add(vertices, nv_Vector2_heap(w, h));
    nv_Array_add(vertices, nv_Vector2_heap(-w, h));

    return nv_Polygon_new(
        type,
        position,
        angle,
        material,
        vertices
    );
}

nv_Array *nv_Polygon_model_to_world(nv_Body *polygon) {
    nv_Array *vertices = nv_Array_new();

    for (size_t i = 0; i < polygon->vertices->size; i++) {
        nv_Vector2 trans = nv_Vector2_add(polygon->position,
                                nv_Vector2_rotate(
                                    *(nv_Vector2 *)polygon->vertices->data[i],
                                    polygon->angle
                                    ));
        nv_Array_add(vertices, nv_Vector2_heapv(trans));
    }

    return vertices;
}