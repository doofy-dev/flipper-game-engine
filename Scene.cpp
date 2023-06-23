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

void Scene::ProcessPhysics(const double &time) {
    for (PhysicsBody *physicsBody: physicsBodies) {
        physicsBody->Process(time);
    }
    auto outerBegin = colliders.begin();
    auto outerEnd = colliders.end();
    for (auto outerIt = outerBegin; outerIt != outerEnd; ++outerIt) {
        ComponentBase *colliderA = *outerIt;
        int typeA = colliderA->getTypeID();
        auto innerBegin = colliders.begin();
        auto innerEnd = colliders.end();
        if (innerBegin == innerEnd) {
            continue;
        }
        for (auto innerIt = innerBegin; innerIt != innerEnd; ++innerIt) {
            ComponentBase *colliderB = *innerIt;
            if(colliderA == colliderB) continue;
            int typeB = colliderB->getTypeID();
            if (typeA == TypeRegistry::getTypeID<CircleCollider>() &&
                typeB == TypeRegistry::getTypeID<CircleCollider>()) {
                handle_collision((CircleCollider *) colliderA, (CircleCollider *) colliderB, time);
            }
         /*   if (typeA == TypeRegistry::getTypeID<CircleCollider>() &&
                typeB == TypeRegistry::getTypeID<PolyCollider>()) {
                handle_collision((PolyCollider *) colliderB, (CircleCollider *) colliderA, time);
            }*/
            if (typeA == TypeRegistry::getTypeID<PolyCollider>() &&
                typeB == TypeRegistry::getTypeID<CircleCollider>()) {
                handle_collision((PolyCollider *) colliderA, (CircleCollider *) colliderB, time);
            }
            if (typeA == TypeRegistry::getTypeID<PolyCollider>() &&
                typeB == TypeRegistry::getTypeID<PolyCollider>()) {
                handle_collision((PolyCollider *) colliderA, (PolyCollider *) colliderB, time);
            }
        }
    }
}

void Scene::handle_collision(CircleCollider *colliderA, CircleCollider *colliderB, const double &time) {
    UNUSED(time);
    CollisionInfo info = colliderA->resolve(colliderB);
    if (!info.is_collided) return;
    if (colliderA->get_trigger()) {
        colliderA->get_entity()->OnCollide(colliderB->get_entity()->get_transform());
        return;
    }
    if (colliderB->get_trigger()) {
        colliderB->get_entity()->OnCollide(colliderA->get_entity()->get_transform());
        return;
    }
    if ((info.a->is_fixed && info.b->is_fixed)) return;
    LOG_D("Collide cc %s %s", info.a->get_entity()->getName(), info.b->get_entity()->getName());
    PhysicsBody::fix_position(&info);
/*    PhysicsBody::resolve_bounce(&info);
    PhysicsBody::resolve_friction(&info, time);*/
}

void Scene::handle_collision(PolyCollider *colliderA, CircleCollider *colliderB, const double &time) {
    UNUSED(time);
    CollisionInfo info = colliderA->resolve(colliderB);
    if (!info.is_collided) {
        return; }
    if (colliderA->get_trigger()) {
        colliderA->get_entity()->OnCollide(colliderB->get_entity()->get_transform());
        return;
    }
    if (colliderB->get_trigger()) {
        colliderB->get_entity()->OnCollide(colliderA->get_entity()->get_transform());
        return;
    }
    if ((info.a->is_fixed && info.b->is_fixed)) return;
    LOG_D("Collide pc %s %s %f %f %f %f", info.a->get_entity()->getName(), info.b->get_entity()->getName(),
          (double)info.a->get_entity()->get_transform()->world_position().x,
          (double)info.a->get_entity()->get_transform()->world_position().y,
          (double)info.b->get_entity()->get_transform()->world_position().x,
          (double)info.b->get_entity()->get_transform()->world_position().y
          );

    PhysicsBody::fix_position(&info);
    info.a->check_sleep();
    info.b->check_sleep();
    if(info.need_bounce)
        PhysicsBody::resolve_bounce(&info);
/*    PhysicsBody::resolve_friction(&info, time);*/
}

void Scene::handle_collision(PolyCollider *colliderA, PolyCollider *colliderB, const double &time) {
    UNUSED(time);
    CollisionInfo info = colliderA->resolve(colliderB);
    if (!info.is_collided) return;
    if (colliderA->get_trigger()) {
        colliderA->get_entity()->OnCollide(colliderB->get_entity()->get_transform());
        return;
    }
    if (colliderB->get_trigger()) {
        colliderB->get_entity()->OnCollide(colliderA->get_entity()->get_transform());
        return;
    }
    if ((info.a->is_fixed && info.b->is_fixed)) return;
    LOG_D("Collide pp %s %s", info.a->get_entity()->getName(), info.b->get_entity()->getName());

    PhysicsBody::fix_position(&info);
/*    PhysicsBody::resolve_bounce(&info);
    PhysicsBody::resolve_friction(&info, time);*/
}
