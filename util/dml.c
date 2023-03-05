#include "dml.h"
#include <math.h>
#include <furi.h>

float lerp(float v0, float v1, float t) {
    if (t > 1) return v1;
    return (1 - t) * v0 + t * v1;
}

void lerp_2d(const Vector *const start, const Vector *const end, float t, Vector *dst) {
    dst->x = lerp(start->x, end->x, t);
    dst->y = lerp(start->x, end->y, t);
}


float vector_cross(const Vector *const a, const Vector *b) {
    return a->x * b->y - a->y * b->x;
}

void
quadratic_2d(const Vector *const start, const Vector *const control, const Vector *const end, float t, Vector *dst) {
    Vector a, b;
    lerp_2d(start, control, t, &a);
    lerp_2d(control, end, t, &b);
    lerp_2d(&a, &b, t, dst);
}

void vector_perpendicular(const Vector *const a, Vector *dst) {
    dst->x = -a->y;
    dst->y = a->x;
}

void vector_add(const Vector *const a, const Vector *const b, Vector *dst) {
    dst->x = a->x + b->x;
    dst->y = a->y + b->y;
}

void vector_sub(const Vector *const a, const Vector *const b, Vector *dst) {
    dst->x = a->x - b->x;
    dst->y = a->y - b->y;
}

void vector_mul_components(const Vector *const a, const Vector *const b, Vector *dst) {
    dst->x = a->x * b->x;
    dst->y = a->y * b->y;
}

void vector_mul(const Vector *const a, float amount, Vector *dst) {
    dst->x = a->x * amount;
    dst->y = a->y * amount;
}

void vector_div_components(const Vector *const a, const Vector *const b, Vector *dst) {
    dst->x = a->x / b->x;
    dst->y = a->y / b->y;
}

void vector_div(const Vector *const a, float b, Vector *dst) {
    dst->x = a->x / b;
    dst->y = a->y / b;
}

void vector_normalized(const Vector *const a, Vector *dst) {
    float length = vector_magnitude(a);
    dst->x = a->x / length;
    dst->y = a->y / length;
}

float vector_magnitude(const Vector *const a) {
    return sqrt(a->x * a->x + a->y * a->y);
}

float vector_distance(const Vector *const a, const Vector *b) {
    Vector c;
    vector_sub(a, b, &c);
    return vector_magnitude(&c);
}

float vector_dot_normalized(const Vector *const a, const Vector *b) {
    Vector _a, _b;
    vector_normalized(a, &_a);
    vector_normalized(b, &_b);
    return _a.x * _b.x + _a.y * _b.y;
}

float vector_dot(const Vector *const a, const Vector *b) {
    return a->x * b->x + a->y * b->y;
}

void vector_rotate(const Vector *const a, float degrees, Vector *dst) {
    dst->x = (float) cos(degrees) * a->x - (float) sin(degrees) * a->y;
    dst->y = (float) sin(degrees) * a->x + (float) cos(degrees) * a->y;
}

bool vector_project(const Vector *const lineA, const Vector *const lineB, const Vector *const point, Vector *dst) {
    Vector AB, AC;
    vector_sub(point, lineA, &AB);
    vector_sub(lineB, lineA, &AC);

    float k = vector_dot(&AC, &AB) / vector_dot(&AB, &AB);
    if (l_abs(k) <= 0 || l_abs(k) > 1) return false;

    dst->x = k * AB.x + lineA->x;
    dst->y = k * AB.y + lineA->y;
    return true;
}

void identity_matrix(Matrix *m) {
    m->data[0] = 1;
    m->data[1] = 0;
    m->data[2] = 0;
    m->data[3] = 0;
    m->data[4] = 1;
    m->data[5] = 0;
    m->data[6] = 0;
    m->data[7] = 0;
    m->data[8] = 1;
}

void translation_matrix(const Vector *const pos, Matrix *m) {
    m->data[0] = 1;
    m->data[1] = 0;
    m->data[2] = pos->x;

    m->data[3] = 0;
    m->data[4] = 1;
    m->data[5] = pos->y;

    m->data[6] = 0;
    m->data[7] = 0;
    m->data[8] = 1;
}

void scale_matrix(float scale, Matrix *m) {
    m->data[0] = scale;//x
    m->data[1] = 0;
    m->data[2] = 0;

    m->data[3] = 0;
    m->data[4] = scale;//y
    m->data[5] = 0;

    m->data[6] = 0;
    m->data[7] = 0;
    m->data[8] = 1;
}

void rotation_matrix(float angle, Matrix *m) {
    m->data[0] = (float) cos(angle);
    m->data[1] = -(float) sin(angle);
    m->data[2] = 0;

    m->data[3] = (float) sin(angle);
    m->data[4] = (float) cos(angle);
    m->data[5] = 0;

    m->data[6] = 0;
    m->data[7] = 0;
    m->data[8] = 1;
}

void matrix_multiply(const Matrix *const a, const Matrix *const b, Matrix *dst) {
    uint8_t i, j, k;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            uint8_t resultID = i * 3 + j;
            dst->data[resultID] = 0;
            for (k = 0; k < 3; k++) {
                dst->data[resultID] += a->data[i * 3 + k] * b->data[k * 3 + j];
            }
        }
    }
}

void get_matrix_translation(const Matrix *const m, Vector *dst) {
    dst->x = m->data[2];
    dst->y = m->data[5];
}

float get_matrix_rotation(const Matrix *const m) {
    return atan2(m->data[0], m->data[1]);
}

void get_matrix_scale(const Matrix *const a, Vector *dst) {
    const float *M = a->data;
    dst->x = sqrt(M[0] * M[0] + M[1] * M[1]);
    dst->y = sqrt(M[3] * M[3] + M[4] * M[4]);
}

void matrix_mul_vector(const Matrix *const a, const Vector *const v, Vector *dst) {
    const float *M = a->data;

    dst->x = M[0] * v->x + M[1] * v->y + M[2];
    dst->y = M[3] * v->x + M[4] * v->y + M[5];
}
