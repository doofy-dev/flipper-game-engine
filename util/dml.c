#include "dml.h"
#include <math.h>
#include <furi.h>

float lerp(float v0, float v1, float t) {
    if (t > 1) return v1;
    return (1 - t) * v0 + t * v1;
}

Vector lerp_2d(Vector start, Vector end, float t) {
    return (Vector) {
            lerp(start.x, end.x, t),
            lerp(start.y, end.y, t),
    };
}

Vector quadratic_2d(Vector start, Vector control, Vector end, float t) {
    return lerp_2d(
            lerp_2d(start, control, t),
            lerp_2d(control, end, t),
            t
    );
}

Vector vector_add(Vector a, Vector b) {
    return (Vector) {a.x + b.x, a.y + b.y};
}

Vector vector_sub(Vector a, Vector b) {
    return (Vector) {a.x - b.x, a.y - b.y};
}

Vector vector_mul_components(Vector a, Vector b) {
    return (Vector) {a.x * b.x, a.y * b.y};
}

Vector vector_div_components(Vector a, Vector b) {
    return (Vector) {a.x / b.x, a.y / b.y};
}

Vector vector_normalized(Vector a) {
    float length = vector_magnitude(a);
    return (Vector) {
            a.x / length,
            a.y / length
    };
}

float vector_magnitude(Vector a) {
    return sqrt(a.x * a.x + a.y * a.y);
}

float vector_distance(Vector a, Vector b) {
    return vector_magnitude(vector_sub(a, b));
}

float vector_dot(Vector a, Vector b) {
    Vector _a = vector_normalized(a);
    Vector _b = vector_normalized(b);
    return _a.x * _b.x + _a.y * _b.y;
}

Vector vector_rotate(Vector a, float degrees) {
    return (Vector) {
            (float) cos(degrees) * a.x - (float) sin(degrees) * a.y,
            (float) sin(degrees) * a.x + (float) cos(degrees) * a.y,
    };
}

Matrix identity_matrix() {
    return (Matrix) {
            .data={
                    1, 0, 0,
                    0, 1, 0,
                    0, 0, 1,
            }
    };
}

Matrix translation_matrix(const Vector *pos) {
    return (Matrix) {
            .data={
                    1, 0, pos->x,
                    0, 1, pos->y,
                    0, 0, 1
            }
    };
}

Matrix scale_matrix(const Vector *scale) {
    return (Matrix) {
            .data={
                    scale->x, 0, 0,
                    0, scale->y, 0,
                    0, 0, 1
            }
    };
}

Matrix rotation_matrix(float angle) {
    return (Matrix) {
            .data={
                    (float) cos(angle), -(float) sin(angle), 0,
                    (float) sin(angle), (float) cos(angle), 0,
                    0, 0, 1,
            }
    };
}

Matrix matrix_multiply(const Matrix *a, const Matrix *b) {
    Matrix m = (Matrix) {.data={0, 0, 0, 0, 0, 0, 0, 0, 0}};
    for (uint8_t row = 0; row < 3; row++) {
        for (uint8_t col = 0; col < 3; col++) {
            for (uint8_t i = 0; i < 3; i++) {
                m.data[row * 3 + col] += a->data[row * 3 + i] * b->data[i * 3 + col];
            }
        }
    }
    return m;
}

Vector get_matrix_translation(const Matrix *m) {
    return (Vector) {m->data[2], m->data[5]};
}

Vector matrix_mul_vector(const Matrix *a, const Vector *v) {
    float r[3] = {0, 0, 0};
    float vec[3] = {v->x, v->y, 1};
    for (uint8_t row = 0; row < 3; row++) {
        for (uint8_t col = 0; col < 3; col++) {
            for (uint8_t i = 0; i < 3; i++) {
                r[row] += a->data[row * 3 + i] * vec[i];
            }
        }
    }
    return (Vector) {vec[0], vec[1]};
}
