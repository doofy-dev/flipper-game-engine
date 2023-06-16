#pragma once

#include <string.h>
#include "types/Transform.h"
#include <gui/icon.h>
#include <furi_hal_resources.h>
#include "types/Component.h"
#include "Helpers.h"
#include "types/Input.h"
#include "physics/PhysicsBody.h"
#include "physics/CircleCollider.h"
#include "physics/PolyCollider.h"

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
    ComponentBase *collider;

    void StartComponent(ComponentBase *component);

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
        if (t->getTypeID() == TypeRegistry::getTypeID<PhysicsBody>()) {
            physicsBody = static_cast<PhysicsBody *>(t);
        } else if (t->getTypeID() == TypeRegistry::getTypeID<CircleCollider>() ||
                   t->getTypeID() == TypeRegistry::getTypeID<PolyCollider>()) {
            collider = t;
        } else {
            components.add(t);
        }
    }

    template<typename T>
    T *GetComponent() {

        if (TypeRegistry::getTypeID<T>() == physicsBody->getTypeID())
            return physicsBody;

        if (TypeRegistry::getTypeID<T>() == collider->getTypeID())
            return collider;

        for (ComponentBase *component: components) {
            if (component->getTypeID() == TypeRegistry::getTypeID<T>())
                return static_cast<T *>(component);
        }
        return nullptr;
    }

    template<typename T>
    void RemoveComponent() {
        if (TypeRegistry::getTypeID<T>() == physicsBody->getTypeID())
            delete physicsBody;

        if (TypeRegistry::getTypeID<T>() == collider->getTypeID())
            delete collider;
        auto item = components.start;
        auto prev = components.start;
        while (item) {
            auto *temp = item->next;
            if (item->data) {
                if (item->data->getTypeID() == TypeRegistry::getTypeID<T>()) {
                    item->data->Destroy();
                    delete item->data;
                    delete item;
                    if (prev == components.start) {
                        components.start = temp;
                    } else {
                        prev->next = temp;
                    }
                    return;
                }
            }
            prev = item;
            item = temp;
        }

        for (ComponentBase *component: components) {
            if (component->getTypeID() == TypeRegistry::getTypeID<T>())
                return static_cast<T *>(component);
        }
    }

    void set_active(bool active);

    bool is_active() const;

    void set_sprite(const Icon *icon);

    Sprite *get_sprite();

    PhysicsBody *getPhysicsBody() { return physicsBody; }
};
