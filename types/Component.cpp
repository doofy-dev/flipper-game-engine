#include "Component.h"
#include "../Engine.h"

InputState ComponentBase::GetInput(InputKey key) {
    return Engine::get_instance()->GetInput(key);
}

void ComponentBase::set_entity(Entity *e) {
    entity = e;
}
