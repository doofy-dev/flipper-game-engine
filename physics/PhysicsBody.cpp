//
// Created by teeebor on 2023-06-14.
//

#include "PhysicsBody.h"
#include "../Entity.h"
#include "../Engine.h"
#include "../types/Transform.h"


void PhysicsBody::Process(const double &delta) {
    //run the position update
    if (is_fixed || sleeping) return;

    Vector up{0, 1};
    Vector friction{0, 0};
    Vector position = entity->get_transform()->local_position();

    Vector tmp = gravity * delta;
    acceleration += tmp;

    float magnitude = velocity.magnitude();
    float normal = gravity.dot(up);

    float friction_coeff, friction_magnitude;
    if (magnitude > 0) {
        friction_coeff = material.dynamic_friction;
        friction_magnitude = friction_coeff * normal;
    } else {
        friction_coeff = material.static_friction;
        friction_magnitude = friction_coeff * normal;
    }
    if (friction_magnitude > acceleration.magnitude()) {
        friction = acceleration * -1;
    } else {
        friction = velocity.normalized() * -friction_magnitude;
    }

//    acceleration += friction;
    velocity += (acceleration + friction) * ((delta * delta) / (double)2.0);
    acceleration.x = 0;
    acceleration.y = 0;
    Vector new_position = position + velocity * delta;
    prev_position = position;

    entity->get_transform()->set_position(new_position);
    entity->get_transform()->update_matrix();
}


void PhysicsBody::fix_position(CollisionInfo *info) {
    if ((info->a->is_fixed && info->b->is_fixed)) return;

    if (!info->a->is_fixed) {
        info->a->entity->get_transform()->move(info->normal * info->depth);
    } else if (!info->b->is_fixed) {
        info->b->entity->get_transform()->move(info->normal * -info->depth);
    } else {
        Vector correctionVec = {
                (float) (info->normal.x * info->depth * 0.5f),
                (float) (info->normal.y * info->depth * 0.5f)
        };
        info->a->entity->get_transform()->move({correctionVec.x, correctionVec.y});
        info->b->entity->get_transform()->move({-correctionVec.x, -correctionVec.y});
    }
}

void PhysicsBody::resolve_bounce(CollisionInfo *info) {
    if ((info->a->is_fixed && info->b->is_fixed)) return;

    float inv_mass1 = (info->a->mass > 0) ? 1.0f / info->a->mass : 0.0f; // Inverse mass of object 1
    float inv_mass2 = (info->b->mass > 0) ? 1.0f / info->b->mass : 0.0f; // Inverse mass of object 2
    Vector deltaVelocity = (info->a->velocity - info->b->velocity).normalized();

    float cor = (info->a->material.bounciness + info->b->material.bounciness) / 2.0f;
    float dot = deltaVelocity.dot(info->normal);
    float impulse = -((1 + cor) * dot) / (inv_mass1 + inv_mass2);

    if (!info->a->is_fixed) {
        info->a->velocity += info->normal * impulse * inv_mass1;
    }
    if (!info->b->is_fixed) {
        info->b->velocity -= info->normal * impulse * inv_mass2;
    }
}

void PhysicsBody::resolve_friction(CollisionInfo *info, const float &delta) {
    if ((info->a->is_fixed && info->b->is_fixed)) return;

    // Project the gravity vector onto the contact normal vector
    float gravity_projection1 = info->a->gravity.x * info->normal.x + info->a->gravity.y * info->normal.y;
    float gravity_projection2 = info->b->gravity.x * info->normal.x + info->b->gravity.y * info->normal.y;

    // Calculate the normal force acting on the objects based on their masses and the gravity projection
    float normal_force1 = -info->a->mass * gravity_projection1;
    float normal_force2 = -info->b->mass * gravity_projection2;

    Vector relative_vel = info->a->velocity - info->b->velocity;
    float normal_force = relative_vel.x * info->normal.y + relative_vel.y * info->normal.y;
    Vector normal_component = info->normal * normal_force;
    Vector tangent_velocity = relative_vel - normal_component;
    Vector normalized_tangent = tangent_velocity.normalized();

    float friction = 0;
    if (!info->a->is_fixed) {
        friction = normal_force1 * info->a->material.dynamic_friction * delta;
        if (fabs(friction) > 1)
            friction = friction < 0 ? -1 : 1;

        info->a->add_force(normalized_tangent * (-friction));
    }
    if (!info->b->is_fixed) {
        friction = normal_force2 * info->b->material.dynamic_friction * delta;
        if (fabs(friction) > 1)
            friction = friction < 0 ? -1 : 1;

        info->b->add_force(normalized_tangent * (-friction));
    }
}


PhysicsBody::PhysicsBody(const Vector &g, float m, PhysicsMaterial pm, bool f) : material(pm) {
    gravity = g;
    mass = m;
    is_fixed = f;
    inertia = 0;
}

void PhysicsBody::add_force(const Vector &force) {
    velocity += force;
}

void PhysicsBody::Start() {
    entity->GetScene()->AddPhysicsBody(this);
    loaded = true;
}

void PhysicsBody::Destroy() {
    entity->GetScene()->RemovePhysicsBody(this);
}

void PhysicsBody::check_sleep() {
    if((prev_position-get_entity()->get_transform()->local_position()).magnitude()<PHYSICS_SLEEP_MARGIN){
        sleep();
    }
}