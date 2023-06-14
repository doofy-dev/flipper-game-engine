#include "Transform.h"

Vector Transform::world_position() {
    return (Vector) {
            model_matrix.data[2],
            model_matrix.data[5]
    };
}

Vector Transform::local_position() {
    return position;
}

void Transform::set_entity(Entity *e) {
    entity = e;
}

void Transform::add_child(Transform *e) {
    children.add(e);
    e->set_parent(this);
}

void Transform::set_parent(Transform *t) {
    parent = t;
}

void Transform::move(const Vector &amount) {
    set_position(position + amount);
}

void Transform::set_position(const Vector &p) {
    position = p;
    dirty = true;
}

void Transform::rotate(float amount) {
    set_rotation(rotation + amount);
}

void Transform::set_rotation(float amount) {
    rotation = amount;
    while(rotation<0) rotation+=M_PIX2;
    while(rotation>M_PIX2) rotation-=M_PIX2;

    dirty = true;
}

void Transform::add_scale(float s) {
    set_scale(scale + s);
}

void Transform::set_scale(float s) {
    scale = s;
    dirty = true;
}

void Transform::update_matrix() {
    dirty = false;

    Matrix t_mat = Matrix::TranslationMatrix(position);
    Matrix r_mat = Matrix::RotationMatrix(rotation);
    Matrix s_mat = Matrix::ScaleMatrix(scale);

    model_matrix = t_mat * r_mat * s_mat;

    if (parent) {
        model_matrix = parent->model_matrix * model_matrix;
    }

    auto start = children.start;
    while (start) {
        start->data->update_matrix();
        start = start->next;
    }
}

bool Transform::is_dirty() const {
    return dirty;
}

List<Transform> *Transform::get_children() {
    return &children;
}

Entity *Transform::get_entity() {
    return entity;
}

Matrix Transform::get_matrix() {
    return model_matrix;
}
