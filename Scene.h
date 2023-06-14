
#pragma once


#include <furi_hal_resources.h>
#include "types/List.h"
#include "Entity.h"
#include "types/Input.h"

class Engine;

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
    void Update(const float &delta, Engine *engine);
    void ProcessPhysics(const float &time);
    void OnInput(InputKey key, InputState type);
    bool is_started(){return started;}
};
