#include "Component.h"
#include "../Engine.h"

InputState Component::GetInput(InputKey key) {
    return Engine::get_instance()->GetInput(key);
}

void Component::set_entity(Entity *e) {
    entity = e;
}
