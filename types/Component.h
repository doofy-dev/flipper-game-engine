#pragma once

#include <furi.h>
#include "Input.h"
#include "../Helpers.h"

class Entity;
class Transform;

class ComponentBase {
protected:
    Entity *entity = nullptr;
    bool enabled = true;
public:
    void set_entity(Entity *e);

    Entity *get_entity() { return entity; }

    virtual ~ComponentBase() {
        LOG_D("Component cleared");
    }
    virtual void OnCollide(Transform* other) {
        UNUSED(other);
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

    virtual int getTypeID() const = 0;

};

template<typename T>
class Component : public ComponentBase {
public:
    int getTypeID() const override {
        static int typeID = TypeRegistry::getTypeID<T>();
        return typeID;
    }
};