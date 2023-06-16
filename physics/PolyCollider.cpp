#include "PolyCollider.h"
#include "../Entity.h"
#include "CircleCollider.h"

PolyCollider::PolyCollider(Vector *_corners, uint8_t _count) : count(_count) {
    for (uint8_t i = 0; i < _count; i++) {
        corners[i] = _corners[i];
    }
}

CollisionInfo PolyCollider::resolve(CircleCollider *other) {
    CollisionInfo result;
    Vector circlePos = other->get_entity()->get_transform()->world_position();
    auto polyInfo = compute();

    float dist = polyInfo.center.distance(circlePos);

    if (dist > (other->get_radius() + polyInfo.radius))
        return result;

    int c = count;

    //If the poly is only a line, reduce the count to 1 to avoid calculating the same thing twice
    if (count == 2) c = 1;
    Vector va, vb, projected;
    bool success = false;

    for (uint8_t i = 0; i < c; i++) {
        va = corners[i];
        vb = corners[(i + 1) % count];
        projected = circlePos.project(va, vb, &success);
        if (success) {
            float length = projected.distance(circlePos);
            if (length > other->get_radius())
                continue;

            result.depth = other->get_radius() - length;
            result.is_collided = true;
            result.a = this->entity->getPhysicsBody();
            result.b = other->get_entity()->getPhysicsBody();
            result.normal = (circlePos - projected).normalized();

            return result;
        }
    }
    return result;
}

CollisionInfo PolyCollider::resolve(PolyCollider *other) {
    CollisionInfo result;
    auto polyInfo1 = compute();
    auto polyInfo2 = other->compute();

    float dist = polyInfo1.center.distance(polyInfo2.center);
    if (dist > (polyInfo1.radius + polyInfo2.radius))
        return result;

    Vector edgeA, edgeB, offset, projected;
    Vector A1, A2, B1, B2;

    uint8_t aCount = count == 2 ? 1 : count;
    uint8_t bCount = other->count == 2 ? 1 : other->count;

    for (uint8_t i = 0; i < aCount; i++) {
        A1 = corners[i];
        A2 = corners[(i + 1) % count];
        for (uint8_t j = 0; j < bCount; j++) {
            B1 = other->corners[j];
            B2 = other->corners[(j + 1) % other->count];

            edgeA = A2 - A1;
            edgeB = B2 - B1;
            offset = A1 - B1;

            float d = edgeA.cross(edgeB);
            if (d == 0) continue;

            float t = edgeB.cross(offset) / d;

            if (t < 0 || t > 1) continue;

            float s = edgeA.cross(offset) / d;
            if (s < 0 || s > 1) continue;

            result.is_collided = true;
            result.normal = {edgeA.y, -edgeA.x};
            result.normal.normalize();

            projected = A1 + (result.normal * t * edgeA);
            result.depth = projected.magnitude();
            offset = (A1 + (edgeA * t)) - polyInfo2.center;
            if (offset.dot(result.normal) < 0) {
                result.normal *= -1;
            }
            return result;
        }
    }

    return result;
}

PolyComputeResult PolyCollider::compute() {
    PolyComputeResult result;
    float r;
    Vector diff;
    Matrix m = entity->get_transform()->get_matrix();
    for (uint8_t i = 0; i < count; i++) {
        result.corners[i] = m * corners[i];
        result.center += corners[i];
    }
    result.center /= (float) count;
    result.center /= (float) count;

    for (uint8_t i = 0; i < count; i++) {
        diff = result.corners[i] - result.center;
        r = diff.magnitude();
        if (result.radius < r) {
            result.radius = r;
        }
    }

    return result;
}
