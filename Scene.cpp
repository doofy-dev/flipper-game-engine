#include "Scene.h"

Scene::Scene(const char *n) : name(n) {
    started = false;
    entities = List<Entity>();
}

Scene::~Scene() {
    Clear();
}

void Scene::Add(Entity *e) {
    entities.add(e);
    e->SetScene(this);
    if (started)
        e->Start();
}

void Scene::Clear() {
    ListItem<Entity> *e = entities.start;
    while (e) {
        e->data->Destroy();
        e = e->next;
    }
}

void Scene::Update(const uint32_t &delta) {
    ListItem<Entity> *e = entities.start;
    while (e) {
        e->data->Update(delta);
        //dirty transform check
        //recursion
    }
}

void Scene::ProcessPhysics(const uint32_t &time) {

}

void Scene::Start() {
    ListItem<Entity> *e = entities.start;
    while (e) {
        e->data->Start();
        e = e->next;
    }
    started = true;
}
