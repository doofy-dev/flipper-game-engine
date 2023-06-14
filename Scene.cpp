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
    auto *start = t->start;

    if(e->get_sprite()){
        engine->QueueSprite(e->get_sprite(), e->get_transform()->get_matrix());
    }

    while (start) {
        if (start->data->get_entity()->is_active()) {
            start->data->get_entity()->Update(delta);
            UpdateTree(delta, start->data->get_entity(), engine);
        }
        start = start->next;
    }
}

void Scene::Update(const float &delta, Engine *engine) {
    ListItem<Entity> *e = entities.start;
    while (e) {
        if (e->data->is_active()) {
            e->data->Update(delta);
            UpdateTree(delta, e->data, engine);
        }

        e = e->next;
    }
}

void Scene::ProcessPhysics(const float &time) {
    UNUSED(time);
}

void Scene::Start() {
    ListItem<Entity> *e = entities.start;
    while (e) {
        e->data->Start();
        e = e->next;
    }
    started = true;
}

void Scene::OnInput(InputKey key, InputState type) {
    ListItem<Entity> *e = entities.start;
    while (e) {
        e->data->OnInput(key, type);
        e = e->next;
    }
}
