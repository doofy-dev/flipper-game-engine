#pragma once

#include "../Entity.h"

class Component {
    Entity *entity = nullptr;
protected:
    bool enabled=true;
public:
    explicit Component(Entity *e):entity(e) {}

    virtual void Start() {}

    virtual void Destroy() {}

    virtual void Update(const uint32_t &delta) {}

    bool is_enabled() const{return enabled;}
};