//
// Created by teeebor on 2023-06-06.
//

#pragma once


#include "Vector.h"
#include "List.h"
#include "Matrix.h"

class Entity;

class Transform {
protected:
    Vector position = {0, 0};
    float scale = 1;
    float rotation = 0;
    List<Transform> children;
    Matrix model_matrix = Matrix();
    bool dirty = true;

    Entity *entity = nullptr;
    Transform *parent = nullptr;
public:
    Vector world_position();

    Vector local_position();

    void set_entity(Entity *e);

    void add_child(Transform *e);

    void set_parent(Transform *t);

    void move(const Vector &amount);

    void set_position(const Vector &p);

    void rotate(float amount);

    void set_rotation(float amount);

    void add_scale(float scale);

    void set_scale(float scale);

    void update_matrix();

    bool is_dirty() const;
};
