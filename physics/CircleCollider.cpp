#include "CircleCollider.h"
#include "PolyCollider.h"
#include "../Entity.h"
#include "../Engine.h"

CollisionInfo CircleCollider::resolve(CircleCollider *other) {
    CollisionInfo result;
    const Vector& translate_a = entity->get_transform()->world_position();
    const Vector& translate_b = other->entity->get_transform()->world_position();

    result.normal = translate_a - translate_b;

    double distance = result.normal.magnitude();
    double radii = radius + other->radius;
    if (distance > radii) return result;

    result.is_collided = true;
    result.a = entity->getPhysicsBody();
    result.b = other->entity->getPhysicsBody();

    auto normalizedVector = result.normal.normalized();

    if (distance == 0) result.normal = {0, 1};
    else result.normal = normalizedVector;

    result.depth = radii - distance;

    return result;
}

CollisionInfo CircleCollider::resolve(PolyCollider *other) {
    return other->resolve(this);
}

CircleCollider::CircleCollider(float r) : radius(r) {
}

void CircleCollider::Start() {
    compute_area_and_mass();
    entity->GetScene()->AddCollider(this);
}

void CircleCollider::compute_area_and_mass() {
    float radSQ = radius * radius;
    auto *pb = entity->GetComponent<PhysicsBody>();
    pb->mass = M_PI * radSQ * pb->material.density;
    pb->inertia =  (float) (0.5 * pb->mass * radSQ);
}

void CircleCollider::Destroy() {
    entity->GetScene()->RemoveCollider(this);
}
