#pragma once
#include <furi.h>
#include "util/list.h"
#include "util/dml.h"
#include "util/ui.h"

typedef struct Entity entity_t;
typedef struct Transform transform_t;
typedef struct Component component_t;
typedef struct Sprite sprite_t;

struct Transform {
    Vector position;
    Vector scale;
    float rotation;
    entity_t *entity;
    entity_t *parent;
    List *children;
    Matrix modelMatrix;
    bool dirty;
};

typedef struct{
    void *data;
    entity_t *entity;
}ComponentInfo;

struct Component {
    ComponentInfo componentInfo;

    void (*start)(ComponentInfo *component, void *game_state);

    void (*update)(ComponentInfo *component, void *game_state);
};

//multiple sprite type implementation needed
//maybe scale and rotation??
struct Sprite{
    const uint8_t *data;
    DrawMode draw_mode;
    Vector size;
};


struct Entity {
    transform_t transform;
    List *components;
    sprite_t sprite;

    bool enabled;
    bool draw;
    const char* name;
};

typedef struct {
    const char* name;
    List *entities;
} Scene;

Scene *new_scene(const char* name);
entity_t *new_entity(const char* name);
void add_to_scene(Scene *s, entity_t *entity);
void clear_scene(Scene *scene);
void add_component(entity_t *entity, void (*start)(ComponentInfo *component, void *state), void (*update)(ComponentInfo *component, void *state), size_t data_size);
void add_to_entity(entity_t *parent, entity_t *child);

Vector world_space_pos(entity_t *e);
Vector get_position(entity_t *e);
Vector get_scale(entity_t *e);
float get_rotation(entity_t *e);

void add_position(transform_t *t, Vector amount);
void set_position(transform_t *t, Vector position);
void add_rotation(transform_t *t, float degree);
void set_rotation(transform_t *t, float degree);
void add_scale(transform_t *t, Vector amount);
void set_scale(transform_t *t, Vector scale);
void update_transform(transform_t *t);
