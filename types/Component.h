#pragma once
#include <furi.h>
#include "Input.h"
#include "../Helpers.h"

class Entity;

class Component {
protected:
    Entity *entity = nullptr;
    bool enabled = true;
public:
    void set_entity(Entity *e);

    virtual ~Component() {
        LOG_D("Component cleared");
    }

    virtual void Start() {}

    virtual void Destroy() {}

    virtual void Update(const float &delta) { UNUSED(delta); }

    virtual void OnInput(InputKey key, InputState type) {
        UNUSED(key);
        UNUSED(type);
    }

    bool is_enabled() const { return enabled; }

    InputState GetInput(InputKey key);

};