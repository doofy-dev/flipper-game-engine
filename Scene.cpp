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

void Scene::ProcessPhysics(const float &time) {
    UNUSED(time);
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
