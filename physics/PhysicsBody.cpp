//
// Created by teeebor on 2023-06-14.
//

#include "PhysicsBody.h"
#include "../Entity.h"

void PhysicsBody::Update(const float &delta) {
    //run the position update
    if (is_fixed) return;

    Vector up{0, 1};
    Vector friction{0, 0};
    float half_time_sq = 0.5f * delta * delta;
    Vector tmp = gravity * delta;
    acceleration = tmp + acceleration;

    float normal_force = gravity.normalized().dot(up);
    if (velocity.magnitude() > 0) {
        float friction_coeff = material.dynamic_friction;
        float friction_mag = friction_coeff * normal_force;
        friction = velocity.normalized() * -friction_mag;
    } else {
        float friction_coeff = material.dynamic_friction;
        float friction_mag = friction_coeff * normal_force;

        if (friction_mag > acceleration.magnitude()) {
            friction = acceleration * -1;
        } else {
            friction = acceleration.normalized() * -friction_mag;
        }
    }

    acceleration = acceleration + friction;
    velocity = (acceleration * delta) + velocity;
    Vector new_pos = entity->get_transform()->world_position() + (velocity * delta) + (acceleration * half_time_sq);
    entity->get_transform()->set_position(new_pos);

}

PhysicsBody::PhysicsBody(Vector gravity, float mass, PhysicsMaterial m, bool fixed) {

}
