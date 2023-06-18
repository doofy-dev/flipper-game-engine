//
// Created by teeebor on 2023-06-14.
//

#include "PhysicsBody.h"
#include "../Entity.h"
#include "../Engine.h"
#include "../types/Transform.h"


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

PhysicsBody::PhysicsBody(const Vector& g, float m, PhysicsMaterial pm, bool f): material(pm) {
    gravity = g;
    mass = m;
    is_fixed = f;
    inertia=0;
}

void PhysicsBody::add_force(const Vector& force) {
    velocity += force;
}

void PhysicsBody::fix_position(CollisionInfo *info) {
/*TODO: move trigger check into the physics loop and send trigger event to components
    also, create OnCollide(Transform *other, Vector point)
 */
    auto *a = (Collider*)info->a->entity->GetCollider();
    auto *b = (Collider*)info->a->entity->GetCollider();
    if((info->a->is_fixed && info->b->is_fixed) || (a->get_trigger() || b->get_trigger())) return;

    if(!info->a->is_fixed){
        info->a->entity->get_transform()->move({
            info->normal.x * info->depth,
            info->normal.y * info->depth
        });
    }else if(!info->b->is_fixed){
        info->b->entity->get_transform()->move({
                -info->normal.x * info->depth,
                -info->normal.y * info->depth
        });
    }else{
        Vector correctionVec = {
                info->normal.x * info->depth * 0.5f,
                info->normal.y * info->depth * 0.5f
        };
        info->a->entity->get_transform()->move({
           correctionVec.x,correctionVec.y
        });
        info->b->entity->get_transform()->move({
           -correctionVec.x,-correctionVec.y
        });
    }

}

void PhysicsBody::Start() {
    entity->GetScene()->AddPhysicsBody(this);
}

void PhysicsBody::Destroy() {
    entity->GetScene()->RemovePhysicsBody(this);
}
