#include "objects.h"
#include "util/util.h"

Scene *new_scene(const char *name) {
    FURI_LOG_I("FlipperGameEngine", "Initializing new scene: %s", name);
    Scene *s = allocate(sizeof(Scene));
    s->name = name;
    s->entities = make_list(sizeof(entity_t));
    return s;
}

entity_t *new_entity(const char *name) {
    FURI_LOG_I("FlipperGameEngine", "Initializing new entity: %s", name);
    entity_t *e = allocate(sizeof(entity_t));
    e->enabled = true;
    e->draw = false;
    e->name = name;
    e->transform.position = (Vector) {0, 0};
    e->transform.rotation = 0;
    e->transform.dirty = true;
    e->transform.scale = (Vector) {1, 1};
    e->components = make_list(sizeof(component_t));
    e->sprite.anchor = (Vector) {0, 0};
    e->transform.children = make_list(sizeof(transform_t));
    e->transform.entity = e;
    e->sprite.draw_mode = BlackOnly;
    return e;
}

void add_to_scene(Scene *s, entity_t *entity) {
    FURI_LOG_D("FlipperGameEngine", "Adding entity '%s' to scene '%s'", entity->name, s->name);
    list_add(s->entities, entity);
}

void clear_component_data(List *l) {
    t_ListItem *li = l->start;
    if (li != NULL) {
        while (li) {
            component_t *t = (component_t *) li->data;
            check_pointer(t);
            check_pointer(t->componentInfo.data);
            release(t->componentInfo.data);
            li = li->next;
        }
    }
    check_pointer(l);
    list_free(l);
}

void clear_branch(List *l) {

    if (l == NULL) return;
    t_ListItem *li = l->start;
    check_pointer(li);

    if (li != NULL) {
        while (li) {
            entity_t *t = (entity_t *) li->data;
            FURI_LOG_I("FlipperGameEngine", "Clearing entity: %s", t->name);
            clear_branch(t->transform.children);
            FURI_LOG_I("FlipperGameEngine", "Freeing components");
            check_pointer(t->components);
            clear_component_data(t->components);
            li = li->next;
        }
    }
    list_free(l);
}

void clear_scene(Scene *scene) {
    FURI_LOG_I("FlipperGameEngine", "Clearing scene: %s", scene->name);
    if (!scene) return;
    clear_branch(scene->entities);
    release(scene);
}

void add_component(entity_t *entity, void (*start)(ComponentInfo *component, void *state),
                   void (*update)(ComponentInfo *component, void *state), size_t data_size) {
    FURI_LOG_I("FlipperGameEngine", "Adding component to: %s", entity->name);
    component_t *component = (component_t *) allocate(sizeof(component_t));
    component->update = update;
    component->start = start;
    component->componentInfo.entity = entity;
    if (data_size > 0)
        component->componentInfo.data = allocate(data_size);
    list_add(entity->components, component);
}

bool is_child_of(entity_t *a, entity_t *b) {
    t_ListItem *s = a->transform.children->start;
    do {
        if (s && s->data) {
            if (s->data == b || is_child_of(s->data, b)) {
                return true;
            }
        }
    } while (s != NULL);
    return false;
}

void add_to_entity(entity_t *parent, entity_t *child) {
    FURI_LOG_D("FlipperGameEngine", "Setting %s parent to be %s", child->name, parent->name);
    if (is_child_of(child, parent)) {
        FURI_LOG_E("FlipperGameEngine", "Cannot add to parent. Child already contains reference to parent!");
        return;
    }
    if (child->transform.parent != NULL) {
        list_remove_item(child->transform.parent->transform.children, child);
    }

    list_add(parent->transform.children, child);
    child->transform.parent = parent;
}

Matrix m_translate, m_rotate, m_scale, m_transform;

void update_transform(transform_t *t) {
    t_ListItem *e = t->children->start;
    t->dirty = false;
    translation_matrix(&(t->position), &m_translate);
    rotation_matrix(t->rotation, &m_rotate);
    scale_matrix(&(t->scale), &m_scale);
    matrix_multiply(&m_translate, &m_rotate, &m_transform);
    matrix_multiply(&m_transform, &m_scale, &(t->modelMatrix));

    if (t->parent) {
        Matrix res;
        matrix_multiply(&(t->parent->transform.modelMatrix), &(t->modelMatrix), &(res));
        t->modelMatrix = res;
    }

    while (e) {
        update_transform(&(((entity_t *) e->data)->transform));
        e = e->next;
    }
}

void add_position(transform_t *t, Vector amount) {
    vector_add(&(t->position), &amount, &(t->position));
    t->dirty = true;
}

void set_position(transform_t *t, Vector position) {
    t->position = position;
    t->dirty = true;
}

void add_rotation(transform_t *t, float degree) {
    set_rotation(t, t->rotation + degree);
}

void set_rotation(transform_t *t, float degree) {
    t->rotation = degree;
    t->dirty = true;
}

void add_scale(transform_t *t, Vector amount) {
    vector_add(&(t->scale), &amount, &(t->scale));
    t->dirty = true;
}

void set_scale(transform_t *t, Vector scale) {
    t->scale = scale;
    t->dirty = true;
}

Vector world_space_pos(entity_t *e) {
    return (Vector) {
            e->transform.modelMatrix.data[2],
            e->transform.modelMatrix.data[5]
    };
}

Vector get_position(entity_t *e) {
    return e->transform.position;
}

Vector get_scale(entity_t *e) {
    return e->transform.scale;
}

float get_rotation(entity_t *e) {
    return e->transform.rotation;
}