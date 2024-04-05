#pragma once

#include "global.hpp"

class Geometry{
public:
    static float    radians(float deg);
    static matrix_t translate(const matrix_t& mat, const vec3f_t& vec);
    static matrix_t rotate(const matrix_t &mat, float deg, const vec3f_t &vec);
    static matrix_t scale(const matrix_t& mat, const vec3f_t& vec);
    static matrix_t lookAt(const vec3f_t& eye, const vec3f_t& center, const vec3f_t& up);
    static matrix_t perspective(float fovy, float aspect, float zNear, float zFar);
    static matrix_t ortho(float left, float right, float bottom, float top, float zNear, float zFar);
    static matrix_t viewport(float x, float y, float w, float h, float zNear, float zFar);
};
