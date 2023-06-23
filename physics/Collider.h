#pragma once

class CircleCollider;
class PolyCollider;
class PhysicsBody;

class ColliderBase{
    bool is_trigger = false;
public:
    void set_trigger(bool trigger) { is_trigger = trigger; }
    bool get_trigger() const { return is_trigger; }
};

template <typename T>
class Collider: public Component<T>, public ColliderBase{
public:
    PhysicsBody *physicsBody;
    virtual void compute_area_and_mass(){}

    virtual CollisionInfo resolve(CircleCollider *other){ UNUSED(other); return {};}

    virtual CollisionInfo resolve(PolyCollider *other){ UNUSED(other); return {};}
};