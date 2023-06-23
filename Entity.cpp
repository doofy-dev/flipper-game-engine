#include "Entity.h"
#include "Engine.h"
#include "Scene.h"
#include "types/Sprite.h"

void Entity::SetScene(Scene *s) {
    scene = s;
}

void Entity::Start() {
    started = true;
    for (auto *component: components) {
        component->Start();
    }
    if(transform.is_dirty())
        transform.update_matrix();
    if (collider)
        collider->Start();
    if (physicsBody)
        physicsBody->Start();
}

Entity::~Entity() {
    LOG_D("Removing physicsbody for %s", name);
    if (physicsBody)
        delete physicsBody;
    LOG_D("Removing collider for %s", name);
    if (collider)
        delete collider;

    LOG_D("Removing components for %s", name);
    auto start = components.start;
    while (start) {
        auto *t = start->next;
        if (start->data) {
            check_pointer(start->data);
            start->data->Destroy();
            delete start->data;
        }
        delete start;
        start = t;
    }
    LOG_D("Entity %s destroyed", name);
}

void Entity::Update(const float &delta) {

    for (auto *component: components) {
        if (component->is_enabled())
            component->Update(delta);
    }
    if (transform.is_dirty()) {
        transform.update_matrix();
    }
}

Entity::Entity(const char *n) {
    name = n;
}

void Entity::set_active(bool active) {
    enabled = active;
}

bool Entity::is_active() const {
    return enabled;
}

void Entity::set_sprite(const Icon *icon) {
    sprite = Engine::get_instance()->LoadSprite(const_cast<Icon *>(icon));
}

Sprite *Entity::get_sprite() {
    return sprite;
}

void Entity::OnInput(InputKey key, InputState type) {
    for (auto *component: components)
        component->OnInput(key, type);
}

void Entity::StartComponent(ComponentBase *component) {
    if (started)
        component->Start();
}

void Entity::OnCollide(Transform *other) {
    for (auto *component: components) {
        if (component->is_enabled())
            component->OnCollide(other);
    }
}

bool Entity::is_started() const {
    return started;
}
