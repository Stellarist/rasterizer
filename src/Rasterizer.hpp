#pragma once

#include <vector>

#include "global.hpp"
#include "Shader.hpp"

class Rasterizer{
private:
public:
    int                   width, height;
    std::vector<color_t>  frame_buffer;
    std::vector<float>    z_buffer;
    matrix_t              model;
    matrix_t              view;
    matrix_t              projection;

public:
    Rasterizer()=default;
    Rasterizer(int width, int height);

    void  clear();
    void  clear(color_t color);
    void  resize(int width, int height);
    void  setPixel(int x, int y, const color_t &color);
    void  setPixel(const vertex_t& point, const color_t& color);
    bool  setDepth(int x, int y, int z);
    int   getIndex(int x, int y) const;
    void* getFramebufferData();

    void drawPoint(const vertex_t& point, const color_t& color={0.f, 0.f, 0.f});
    void drawLine(const line_t& line, const color_t& color={0.f, 0.f, 0.f});
    void drawTriangle(const triangle_t& triangle, ShaderInfo& shader);
    
    static bool isInsideTriangle(int x, int y, const triangle_t& triangle);
    static bool isTriangleBackface(const triangle_t& triangle);
    static auto computeBarycentric(int x, int y, const triangle_t& triangle) -> std::tuple<float, float,float>;

    template<typename T>
    auto interpolate(float alpha, float beta, float gamma, const T& v0, const T& v1, const T& v2) -> T
    { return alpha*v0+beta*v1+gamma*v2; }
};
