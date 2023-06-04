
#pragma once


#include "types/List.h"
#include "Entity.h"

class Scene {
    List<Entity> entities;
    const char* name;
    bool started;

public:
    Scene(const char* name);
    ~Scene();

    void Add(Entity *e);
    void Clear();
    void Start();
    void Update(const uint32_t &delta);
    void ProcessPhysics(const uint32_t &time);
};
