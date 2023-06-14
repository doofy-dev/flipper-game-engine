#include "Entity.h"
#include "Engine.h"
#include "Scene.h"
#include <furi.h>
#include "types/Sprite.h"

void Entity::SetScene(Scene *s) {
    scene = s;
}

void Entity::Start() {
    auto start = components.start;
    while (start) {
        start->data->Start();
        start = start->next;
    }
}

Entity::~Entity() {
    LOG_D("Entity %s destroyed", name);
    if (physicsBody)
        delete physicsBody;

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
}

void Entity::Update(const float &delta) {
    auto start = components.start;
    while (start) {
        if (start->data->is_enabled())
            start->data->Update(delta);
        if (transform.is_dirty()) {
            transform.update_matrix();
        }
        start = start->next;
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
    auto *start = components.start;
    while (start) {
        start->data->OnInput(key, type);
        start = start->next;
    }
}
