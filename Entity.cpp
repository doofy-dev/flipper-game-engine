#include "Entity.h"
#include "Scene.h"
#include "types/Component.h"
#include <type_traits>

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
    auto start = components.start;
    while (start) {
        start->data->Destroy();
        start = start->next;
    }
}

void Entity::Update(const uint32_t &delta) {
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

template<typename T>
void Entity::AddComponent() {
    static_assert(std::is_base_of<Component, T>::value, "Class must be derived from Component");
    components.add(new T(this));
}
