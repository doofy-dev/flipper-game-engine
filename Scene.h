
#pragma once


#include <furi_hal_resources.h>
#include "types/List.h"
#include "Entity.h"
#include "types/Input.h"

class Engine;

class Scene {
    List<Entity> entities;
    List<ComponentBase> colliders;
    List<PhysicsBody> physicsBodies;
    const char* name;
    bool started;

public:
    explicit Scene(const char* name);
    ~Scene();

    void Add(Entity *e);
    void Clear();
    void Start();
    void Update(const float &delta, Engine *engine);
    void ProcessPhysics(const float &time);
    void OnInput(InputKey key, InputState type);
    bool is_started() const{return started;}

    void AddCollider(ComponentBase *collider);
    void RemoveCollider(ComponentBase *collider);

    void AddPhysicsBody(PhysicsBody *physicsBody);
    void RemovePhysicsBody(PhysicsBody *physicsBody);
};
