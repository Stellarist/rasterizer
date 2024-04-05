#pragma once

#include <array>
#include <eigen3/Eigen/Eigen>

using vertex_t   = Eigen::Vector3f;
using normal_t   = Eigen::Vector3f;
using texcoord_t = Eigen::Vector2f;
using color_t    = Eigen::Vector3f;
using matrix_t   = Eigen::Matrix4f;
using direct_t   = Eigen::Vector3f;
using mat3f_t    = Eigen::Matrix3f;
using vec2f_t    = Eigen::Vector2f;
using vec3f_t    = Eigen::Vector3f;
using vec4f_t    = Eigen::Vector4f;
using line_t     = std::array<vertex_t, 2>;

using triangle_t = struct {
    std::array<vertex_t, 3>   vertices;
    std::array<normal_t, 3>   normals;
    std::array<texcoord_t, 3> texcoords;
    std::array<color_t, 3>    colors;
};

using light_t = struct {
    vec3f_t position;
    vec3f_t intensity;
};

#ifdef DEBUG
#include <iostream>
inline void print(const triangle_t& triangle)
{
    static int cnt=0;
    cnt++;
    std::cout<<"vertex:"<<std::endl;
    for(int i=0; i<3; i++){
        std::cout<<triangle.vertices[i].x()<<" ";
        std::cout<<triangle.vertices[i].y()<<" ";
        std::cout<<triangle.vertices[i].z()<<std::endl;
    }

    std::cout<<"normal:"<<std::endl;
    for(int i=0; i<3; i++){
        std::cout<<triangle.normals[i].x()<<" ";
        std::cout<<triangle.normals[i].y()<<" ";
        std::cout<<triangle.normals[i].z()<<std::endl;
    }

    std::cout<<"texcoord:"<<std::endl;
    for(int i=0; i<3; i++){
        std::cout<<triangle.texcoords[i].x()<<" ";
        std::cout<<triangle.texcoords[i].y()<<std::endl;
    }

    std::cout<<"color:"<<std::endl;
    for(int i=0; i<3; i++){
        std::cout<<triangle.colors[i].x()<<" ";
        std::cout<<triangle.colors[i].y()<<" ";
        std::cout<<triangle.colors[i].z()<<std::endl;
    }

    std::cout<<cnt<<std::endl;
}
#endif
