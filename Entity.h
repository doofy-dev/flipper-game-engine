#pragma once
#include "furi.h"

class Scene;

class Entity {
    Scene *scene = nullptr;
public:
    virtual void Start() {  }
    virtual void Destroy() {  }
    virtual void Update(const uint32_t &delta) {  }

    void SetScene(Scene *s);
};
