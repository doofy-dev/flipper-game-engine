#pragma once

#include <string.h>
#include "types/Transform.h"
#include <gui/icon.h>
#include <furi_hal_resources.h>
#include "types/Component.h"
#include "Helpers.h"
#include "types/Input.h"
#include "physics/PhysicsBody.h"

class Scene;

class Sprite;

class Entity {
    const char *name;
    Scene *scene = nullptr;
    Transform transform;
    List<ComponentBase> components;
    bool enabled = true;
    Sprite *sprite;
    PhysicsBody *physicsBody;
    Collider *collider;

public:
    Entity(const char *name);

    void Start();

    ~Entity();

    void OnInput(InputKey key, InputState type);

    void Update(const float &delta);

    Transform *get_transform() { return &transform; }

    void SetScene(Scene *s);

    template<typename T>
    void AddComponent() {
        T *t = new T();
        t->set_entity(this);
        components.add(t);
    }

    template<typename T, typename... Args>
    void AddComponent(Args &&... args) {
        ComponentBase *t = new T(args...);
        t->set_entity(this);
        components.add(t);
        if (t->getTypeID() == TypeRegistry::getTypeID<PhysicsBody>()) {
            physicsBody = static_cast<PhysicsBody *>(t);
        }
    }

    template<typename T>
    T *GetComponent() {
        for (ComponentBase *component: components) {
            if (component->getTypeID() == TypeRegistry::getTypeID<T>())
                return static_cast<T *>(component);
        }
        return nullptr;
    }

    void set_active(bool active);

    bool is_active() const;

    void set_sprite(const Icon *icon);

    Sprite *get_sprite();

    PhysicsBody *getPhysicsBody() { return physicsBody; }
};
