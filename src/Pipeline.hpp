#pragma once

#include "Camera.hpp"
#include "Model.hpp"
#include "Rasterizer.hpp"
#include "Shader.hpp"

class Pipeline{
public:
    static Camera*     camera_ptr;
    static Model*      model_ptr;
    static Rasterizer* rasterizer_ptr;
    static Shader*     shader_ptr;

    static bool valid();
    static void bind(Camera* camera_ptr);
    static void bind(Model* model);
    static void bind(Rasterizer* rasterizer_ptr);
    static void bind(Shader* shader_ptr);
    static void clear(color_t color=color_t{0.f, 0.f, 0.f});
    static void render();
};