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
    float massA = collision->a->mass;
    float massB = collision->b->mass;
    float bouncinessA = collision->a->material.bounciness;
    float bouncinessB = collision->b->material.bounciness;
    Vector velocityA = collision->a->velocity;
    Vector velocityB = collision->b->velocity;

    float invMassA = massA == 0 ? 1 : 1 / massA;
    float invMassB = massB == 0 ? 1 : 1 / massB;

    float bounciness = (bouncinessA + bouncinessB) / 2;
    float impulse = (-(1 + bounciness) * vector_dot(&velocityA, &collision->normal) +
                     vector_dot(&velocityB, &collision->normal)) / (invMassA + invMassB);

    Vector impulseVec = {
            impulse * collision->normal.x,
            impulse * collision->normal.y
    };

    if (!collision->a->fixed) {
        collision->a->velocity.x += impulseVec.x * invMassA;
        collision->a->velocity.y += impulseVec.y * invMassA;
    }

    if (!collision->b->fixed) {
        collision->b->velocity.x -= impulseVec.x * invMassB;
        collision->b->velocity.y -= impulseVec.y * invMassB;
    }
}

void resolve_friction(CollisionInfo *collision) {
    float friction = min(collision->a->material.dynamic_friction, collision->b->material.dynamic_friction);
    Vector frictionForce = (Vector) {-collision->normal.x * friction, -collision->normal.y * friction};

    if (!collision->a->fixed)
        add_force(collision->a, frictionForce);
    if (!collision->b->fixed)
        add_force(collision->b, frictionForce);
}