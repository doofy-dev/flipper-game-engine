#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-deprecated-headers"
#pragma once

#include <string.h>
#include "furi.h"
#include "types/Transform.h"

class Scene;

class Component;

class Entity {
    const char *name;
    Scene *scene = nullptr;
    Transform transform;
    List<Component> components;
public:
    Entity(const char *name);

    void Start();

    ~Entity();

    void Update(const uint32_t &delta);

    Transform *Transform() { return &transform; }

    void SetScene(Scene *s);

    template<typename T>
    void AddComponent();

    bool operator==(const char *other) const {
        return strcmp(name, other) == 0;
    }
};

#pragma clang diagnostic pop