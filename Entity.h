#pragma once

#include <string.h>
#include <typeinfo>
#include "types/Transform.h"
#include <gui/icon.h>
#include <furi_hal_resources.h>
#include "types/Component.h"
#include "types/Input.h"
#include "physics/PhysicsBody.h"

class Scene;

class Sprite;

class Entity {
    const char *name;
    Scene *scene = nullptr;
    Transform transform;
    List<Component> components;
    bool enabled = true;
    Sprite *sprite;
    PhysicsBody *physicsBody;

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
       /* if (typeid(T) == typeid(PhysicsBody)) {
            physicsBody = new PhysicsBody(args...);
            physicsBody->set_entity(this);
        } else {*/
            T *t = new T(args...);
            t->set_entity(this);
            components.add(t);
//        }
    }

/*
    template<>
    template<typename... Args>
    void AddComponent<PhysicsBody>(Args &&... args){
        physicsBody = new PhysicsBody(args...);
        physicsBody->set_entity(this);
    }
*/

    template<typename T>
    T *GetComponent() {
        auto *t = components.start;
        while (t) {
            if (typeid(*(t->data)) == typeid(T)) {
                return t->data;
            }
            t = t->next;
        }
        return nullptr;
    }

    void set_active(bool active);

    bool is_active() const;

    void set_sprite(const Icon *icon);

    Sprite *get_sprite();

};
