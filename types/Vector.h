#pragma once

#include <cmath>
#include "Helpers.h"

struct Vector {
    float x;
    float y;

    Vector &operator=(const Vector &other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
        }
        return *this;
    }

    Vector operator+(Vector const &other) {
        return Vector{x + other.x, y + other.y};
    }

    Vector operator-(Vector const &other) {
        return Vector{x - other.x, y - other.y};
    }

    Vector operator*(Vector const &other) {
        return Vector{x * other.x, y * other.y};
    }

    Vector operator*(float other) {
        return Vector{x * other, y * other};
    }

    Vector operator/(Vector const &other) {
        return Vector{x / other.x, y / other.y};
    }

    Vector operator/(float other) {
        return Vector{x / other, y / other};
    }

    float magnitude() {
        return sqrtf(x * x + y * y);
    }

    float distance(Vector const &other) {
        Vector v = *this - other;
        return v.magnitude();
    }

    void normalize() {
        float m = magnitude();
        if (m == 0) {
            x = 0;
            y = 0;
        }

        x = x / m;
        y = y / m;
    }

    Vector normalized() {
        float m = magnitude();
        if (m == 0) return {0, 0};

        return {x / m, y / m};
    }

    float dot(Vector const &b) {
        return x * b.x + y * b.y;
    }

    void rotate(float deg) {
        x = cos(deg) * x - sin(deg) * y;
        y = sin(deg) * x + cos(deg) * y;
    }

    float cross(Vector const &other) {
        return x * other.x - y * other.y;
    }

    Vector perpendicular() {
        return {-y, x};
    }

    Vector project(Vector const &lineA, Vector const &lineB, bool &success) {
        float distX = lineB.x - lineA.x;
        float distY = lineB.y - lineA.y;
        float len = sqrtf(distX * distX + distY * distY);

        float dot = ((x - lineA.x) * (lineB.x - lineA.x) + (y - lineA.y) * (lineB.y - lineA.y)) / (len * len);
        success = false;

        if (dot < 0 || dot > 1) return {0, 0};
        success = true;

        return {
                lineA.x + (dot * (lineB.x - lineA.x)),
                lineA.y + (dot * (lineB.y - lineA.y))
        };
    }

    static Vector Lerp(Vector const &start, Vector const &end, float time) {
        return {
                lerp(start.x, end.x, time),
                lerp(start.y, end.y, time)
        };
    }

    static Vector Quadratic(Vector const &start, Vector const &control, Vector const &end, float time) {
        Vector a = Vector::Lerp(start, control, time);
        Vector b = Vector::Lerp(control, end, time);
        return Vector::Lerp(a, b, time);
    }
};