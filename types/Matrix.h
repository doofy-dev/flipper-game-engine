#pragma once

#include <cmath>
#include "Vector.h"

struct Matrix {
    float data[9];

    void Identity() {
        data[0] = 1;
        data[1] = 0;
        data[2] = 0;
        data[3] = 0;
        data[4] = 1;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        data[8] = 1;
    }

    static Matrix ScaleMatrix(float s) {
        return {
                {
                        s, 0, 0,
                        0, s, 0,
                        0, 0, s
                }
        };
    }

    static Matrix TranslationMatrix(Vector const &pos) {
        return {
                {
                        1, 0, pos.x,
                        0, 1, pos.y,
                        0, 0, 1
                }
        };
    }

    static Matrix RotationMatrix(float angle) {
        return {
                {
                        (float) (cos(angle)), (float) (-sin(angle)), 0,
                        (float) (sin(angle)), (float) (cos(angle)), 0,
                        0, 0, 1
                }
        };
    }

    Matrix operator*(Matrix const &other) {
        return {
                {
                        data[0] * other.data[0] + data[1] * other.data[3] + data[2] * other.data[6],
                        data[0] * other.data[1] + data[1] * other.data[4] + data[2] * other.data[7],
                        data[0] * other.data[2] + data[1] * other.data[5] + data[2] * other.data[8],

                        data[3] * other.data[0] + data[4] * other.data[3] + data[5] * other.data[6],
                        data[3] * other.data[1] + data[4] * other.data[4] + data[5] * other.data[7],
                        data[3] * other.data[2] + data[4] * other.data[5] + data[5] * other.data[8],

                        data[6] * other.data[0] + data[7] * other.data[3] + data[8] * other.data[6],
                        data[6] * other.data[1] + data[7] * other.data[4] + data[8] * other.data[7],
                        data[6] * other.data[2] + data[7] * other.data[5] + data[8] * other.data[8]
                }
        };
    }

    Vector translation() const{
        return {data[2], data[5]};
    }

    float rotation() const{
        return (float)atan2(data[0], data[1]);
    }

    Vector scale() const {
        return {
                (float) sqrt(data[0] * data[0] + data[1] * data[1]),
                (float) sqrt(data[3] * data[3] + data[4] * data[4])
        };
    }

    Vector operator*(Vector const &other) {
        return {
                data[0] * other.x + data[1] * other.y + data[2],
                data[3] * other.x + data[4] * other.y + data[5]
        };
    }
};