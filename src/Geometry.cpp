#include "Geometry.hpp"

constexpr float PI = 3.14159265358979323846;

float Geometry::radians(float deg)
{
    return deg*PI/180.0f;
}

matrix_t Geometry::translate(const matrix_t& mat, const vec3f_t& vec)
{
    matrix_t res = mat;
    res(0, 3) += vec[0];
    res(1, 3) += vec[1];
    res(2, 3) += vec[2];

    return res;
}

matrix_t Geometry::rotate(const matrix_t &mat, float deg, const vec3f_t &vec)
{
    float norm=vec.norm();
    vec3f_t rot=vec/norm;

    mat3f_t N;
    N<<0, -rot[2], rot[1],
        rot[2], 0, -rot[0],
        -rot[1], rot[0], 0;
    mat3f_t I=mat3f_t::Identity();
    mat3f_t R=I*cos(deg)+rot*rot.transpose()*(1-cos(deg))+N*sin(deg);
    matrix_t res=mat;
    res.block(0, 0, 3, 3)=R*mat.block(0, 0, 3, 3);

    return res;
}

matrix_t Geometry::scale(const matrix_t &mat, const vec3f_t &vec)
{
    matrix_t res = mat;
    res(0, 0) *= vec[0];
    res(1, 1) *= vec[1];
    res(2, 2) *= vec[2];
    
    return res;
}

matrix_t Geometry::lookAt(const vec3f_t &eye, const vec3f_t &center, const vec3f_t &up)
{
    vec3f_t f=(center-eye).normalized();
    vec3f_t s=f.cross(up).normalized();
    vec3f_t u=s.cross(f);

    matrix_t res;
    res<<s.x(), s.y(), s.z(), -s.dot(eye),
        u.x(), u.y(), u.z(), -u.dot(eye),
        -f.x(), -f.y(), -f.z(), f.dot(eye),
        0.f, 0.f, 0.f, 1.f;

    return res;
}

matrix_t Geometry::perspective(float fovy, float aspect, float zNear, float zFar)
{
    float tan_half_fovy=tan(radians(fovy/2));
    matrix_t res;
    res<<1.f/(aspect*tan_half_fovy), 0.f, 0.f, 0.f,
        0.f, 1.f/tan_half_fovy, 0.f, 0.f,
        0.f, 0.f, -(zFar+zNear)/(zFar-zNear), -2*zFar*zNear/(zFar-zNear),
        0.f, 0.f, -1.f, 0.f;

    return res;
}


matrix_t Geometry::ortho(float left, float right, float bottom, float top, float zNear, float zFar)
{
    matrix_t res;
    res<<2.f/(right-left), 0.f, 0.f, -(right+left)/(right-left),
        0.f, 2.f/(top-bottom), 0.f, -(top+bottom)/(top-bottom),
        0.f, 0.f, -2.f/(zFar-zNear), -(zFar+zNear)/(zFar-zNear),
        0.f, 0.f, 0.f, 1.f;
    
    return res;
}
