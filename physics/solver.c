#include "solver.h"
#include "../objects.h"

void fix_position(CollisionInfo *collision) {

    if (collision->a->fixed && collision->b->fixed) return;

    if (!collision->a->fixed) {
        add_position(collision->a->transform, (Vector) {
                collision->normal.x * collision->depth,
                collision->normal.y * collision->depth
        });
    } else if (!collision->b->fixed) {
        add_position(collision->b->transform, (Vector) {
                -collision->normal.x * collision->depth,
                -collision->normal.y * collision->depth
        });
    } else {
        Vector correctionVec = {
                collision->normal.x * collision->depth * 0.5f,
                collision->normal.y * collision->depth * 0.5f
        };
        add_position(collision->a->transform, (Vector) {-correctionVec.x, -correctionVec.y});
        add_position(collision->b->transform, (Vector) {correctionVec.x, correctionVec.y});
    }
}

void resolve_bouncing(CollisionInfo *collision) {
    if (collision->a->fixed && collision->b->fixed) return;

    float inv_mass1 = (collision->a->mass > 0) ? 1.0f / collision->a->mass : 0.0f; // Inverse mass of object 1
    float inv_mass2 = (collision->b->mass > 0) ? 1.0f / collision->b->mass : 0.0f; // Inverse mass of object 2

    Vector deltaVelocity;
    vector_sub(&(collision->a->velocity), &(collision->b->velocity), &deltaVelocity);

    float cor = (collision->a->material.bounciness + collision->b->material.bounciness) / 2.0f;
    float dotProduct = vector_dot(&deltaVelocity, &(collision->normal));
    float impulse = (-(1 + cor) * dotProduct) / (inv_mass1 + inv_mass2);

    if (!collision->a->fixed) {
        collision->a->velocity.x += impulse * inv_mass1 * collision->normal.x;
        collision->a->velocity.y += impulse * inv_mass1 * collision->normal.y;
    }

    if (!collision->b->fixed) {
        collision->b->velocity.x -= impulse * inv_mass2 * collision->normal.x;
        collision->b->velocity.y -= impulse * inv_mass2 * collision->normal.y;
    }
}

void resolve_friction(CollisionInfo *collision, float delta) {
    if (collision->a->fixed && collision->b->fixed) return;

    // Project the gravity vector onto the contact normal vector
    float gravity_projection1 =
            collision->a->gravity.x * collision->normal.x + collision->a->gravity.y * collision->normal.y;
    float gravity_projection2 =
            collision->b->gravity.x * collision->normal.x + collision->b->gravity.y * collision->normal.y;

    // Calculate the normal force acting on the objects based on their masses and the gravity projection
    float normal_force1 = -collision->a->mass * gravity_projection1;
    float normal_force2 = -collision->b->mass * gravity_projection2;

    Vector relative_velocity;
    vector_sub(&(collision->a->velocity), &(collision->b->velocity), &relative_velocity);
    float normalForce = (relative_velocity.x * collision->normal.x + relative_velocity.y * collision->normal.y);
    Vector normal_component = {
            collision->normal.x * normalForce,
            collision->normal.y * normalForce
    };
    Vector tangent_velocity = {
            relative_velocity.x - normal_component.x, relative_velocity.y - normal_component.y
    };
    Vector normalized_tangent_vel;
    vector_normalized(&tangent_velocity, &normalized_tangent_vel);

    // Calculate the friction force based on the normal force and the friction coefficients

    float friction_force;
    if (!collision->a->fixed) {
        friction_force =
                normal_force1 * (/*collision->a->material.static_friction + */collision->a->material.dynamic_friction) * delta;

        if(fabs(friction_force)>1)
            friction_force=friction_force<0?-1:1;

        add_force(collision->a,
                  (Vector) {-normalized_tangent_vel.x * friction_force, -normalized_tangent_vel.y * friction_force});
    }
    if (!collision->b->fixed) {
        friction_force =
                normal_force2 * (/*collision->b->material.static_friction + */collision->b->material.dynamic_friction) * delta;
        if(fabs(friction_force)>1)
            friction_force=friction_force<0?-1:1;
        add_force(collision->b,
                  (Vector) {normalized_tangent_vel.x * friction_force, normalized_tangent_vel.y * friction_force});
    }
}