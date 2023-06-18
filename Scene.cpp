#include "Scene.h"
#include "Entity.h"
#include "types/List.h"
#include "Helpers.h"
#include "Engine.h"

Scene::Scene(const char *n) : name(n) {
    started = false;
    entities = List<Entity>();
}

Scene::~Scene() {
    LOG_D("Scene %s destroyed", name);
    Clear();
}

void Scene::Add(Entity *e) {
    entities.add(e);
    e->SetScene(this);
    if (started)
        e->Start();
}

void Scene::Clear() {
    colliders.soft_clear();
    physicsBodies.soft_clear();
    entities.empty();
}

void UpdateTree(const float &delta, Entity *e, Engine *engine) {
    auto *t = e->get_transform()->get_children();

    if (e->get_sprite()) {
        engine->QueueSprite(e->get_sprite(), e->get_transform()->get_matrix());
    }
    for (auto *transform: *t) {
        transform->get_entity()->Update(delta);
        UpdateTree(delta, transform->get_entity(), engine);
    }
}

void Scene::Update(const float &delta, Engine *engine) {
    for (auto *entity: entities) {
        if (entity->is_active()) {
            entity->Update(delta);
            UpdateTree(delta, entity, engine);
        }
    }

}

void Scene::Start() {
    for (auto *entity: entities)
        entity->Start();

    started = true;
}

void Scene::OnInput(InputKey key, InputState type) {
    for (auto *entity: entities)
        entity->OnInput(key, type);

}

void Scene::AddCollider(ComponentBase *collider) {
    colliders.add(collider);
}

void Scene::RemoveCollider(ComponentBase *collider) {
    colliders.soft_remove(collider);
}

void Scene::AddPhysicsBody(PhysicsBody *physicsBody) {
    physicsBodies.add(physicsBody);
}

void Scene::RemovePhysicsBody(PhysicsBody *physicsBody) {
    physicsBodies.soft_remove(physicsBody);
}

void Scene::ProcessPhysics(const float &time) {
    for (PhysicsBody *physicsBody: physicsBodies) {
        physicsBody->Update(time);
    }
    auto outerBegin = colliders.begin();
    auto outerEnd = colliders.end();
    for (auto outerIt = outerBegin; outerIt != outerEnd; ++outerIt) {
        ComponentBase *colliderA = *outerIt;
        int typeA = colliderA->getTypeID();
        auto innerBegin = colliders.begin();
        auto innerEnd = colliders.end();
        if(innerBegin == innerEnd) {
            continue;
        }
        innerBegin++;
        for (auto innerIt = innerBegin; innerIt != innerEnd; ++innerIt) {
            ComponentBase *colliderB = *innerIt;
            int tpyeB = colliderB->getTypeID();
        }
    }
}
