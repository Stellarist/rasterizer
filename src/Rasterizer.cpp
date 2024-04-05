#include "Rasterizer.hpp"
#include "Shader.hpp"

Rasterizer::Rasterizer(int width, int height)
: width(width), height(height),
  frame_buffer(width*height, {0.f, 0.f, 0.f}),
  z_buffer(width*height, std::numeric_limits<float>::max())
{ }

void Rasterizer::clear()
{
    clear({0.f, 0.f, 0.f});
}

void Rasterizer::clear(color_t color)
{
    std::fill(frame_buffer.begin(), frame_buffer.end(), color);
    std::fill(z_buffer.begin(), z_buffer.end(), std::numeric_limits<float>::max());
}

void Rasterizer::resize(int width, int height)
{
    this->width=width;
    this->height=height;
    frame_buffer.resize(width*height, {0.f, 0.f, 0.f});
    z_buffer.resize(width*height, std::numeric_limits<float>::max());
}

void Rasterizer::setPixel(int x, int y, const color_t &color)
{
    if(x>=width || y>=height || x<0 || y<0)
        return;

    int index=y*width+x;
    frame_buffer[index]=color;
}

void Rasterizer::setPixel(const vertex_t& point, const color_t& color)
{
    int x=static_cast<int>(point.x());
    int y=static_cast<int>(point.y());

    if(x>=width || y>=height || x<0 || y<0)
        return;

    int index=y*width+x;
    frame_buffer[index]=color;
}

bool Rasterizer::setDepth(int x, int y, int z)
{
    int index=getIndex(x, y);
    if(index>z_buffer.size()){
        return false;
    }else if(z>z_buffer[index]){
        return false;
    }else{
        z_buffer[index]=index;
        return true;
    }
}

int Rasterizer::getIndex(int x, int y) const
{
    return y*width+x;
}

void* Rasterizer::getFramebufferData()
{
    return frame_buffer.data();
}

std::tuple<float, float, float> Rasterizer::computeBarycentric(int x, int y, const triangle_t& triangle)
{
    const auto& v=triangle.vertices;
    vec2f_t v0(v[1].x()-v[0].x(), v[1].y()-v[0].y());
    vec2f_t v1(v[2].x()-v[0].x(), v[2].y()-v[0].y());
    vec2f_t v2(x-v[0].x(), y-v[0].y());
    float d00=v0.dot(v0);
    float d01=v0.dot(v1);
    float d11=v1.dot(v1);
    float d20=v2.dot(v0);
    float d21=v2.dot(v1);
    float denom=d00*d11-d01*d01;
    if(denom==0)
        return {1.f/3, 1.f/3, 1.f/3};
    else{
        float beta=(d11*d20-d01*d21)/denom;
        float gamma=(d00*d21-d01*d20)/denom;
        float alpha=1-beta-gamma;
        return {alpha, beta, gamma};
    }
}

void Rasterizer::drawTriangle(const triangle_t& triangle, ShaderInfo& shader_info)
{
    if(isTriangleBackface(triangle))
        return;

    const auto& v=triangle.vertices;
    const auto& n=triangle.normals;
    const auto& t=triangle.texcoords;
    const auto& c=triangle.colors;

    float min_x=std::min({triangle.vertices[0].x(), triangle.vertices[1].x(), triangle.vertices[2].x()});
    float max_x=std::max({triangle.vertices[0].x(), triangle.vertices[1].x(), triangle.vertices[2].x()});
    float min_y=std::min({triangle.vertices[0].y(), triangle.vertices[1].y(), triangle.vertices[2].y()});
    float max_y=std::max({triangle.vertices[0].y(), triangle.vertices[1].y(), triangle.vertices[2].y()});

    for(int x=min_x; x<=max_x; x++){
        for(int y=min_y; y<=max_y; y++){
            // out of screen
            if(x>width && y>height)
                continue;

            auto[alpha, beta, gamma]=computeBarycentric(x, y, triangle);

            // out of triangle
            if(alpha>1 || alpha<0 || beta>1 || beta<0 || gamma>1 || gamma<0)
                continue;
            float z=1.f/(alpha/v[0].z()+beta/v[1].z()+gamma/v[2].z());

            shader_info.normal=interpolate<normal_t>(alpha, beta, gamma, n[0], n[1], n[2]);
            shader_info.color=interpolate<color_t>(alpha, beta, gamma, c[0], c[1], c[2]);
            shader_info.texcoord=interpolate<texcoord_t>(alpha, beta, gamma, t[0], t[1], t[2]);

            if(setDepth(x, y, z))
                setPixel(x, y, Shader::textureShader(shader_info));
        }
    }
}

void Rasterizer::drawLine(const line_t& line, const color_t& color)
{
    bool is_steep=false;
    auto begin=line[0];
    auto end=line[1];

    if(abs(end.x()-begin.x())<abs(end.y()-begin.y())){
        std::swap(begin.x(), begin.y());
        std::swap(end.x(), end.y());
        is_steep=true;
    }
    if(end.x()<begin.x())
        std::swap(begin, end);

    int x=static_cast<int>(begin.x());
    int y=static_cast<int>(begin.y());
    int dx=static_cast<int>(end.x()-begin.x());
    int dy=static_cast<int>(abs(end.y()-begin.y()));
    int e=-dx, d=end.y()>begin.y() ? 1 : -1;

    for(int i=0; i<dx; i++){
        if(is_steep)
            setPixel(y, x, color);
        else
            setPixel(x, y, color);
        x++;
        e+=2*dy;
        if(e>0){
            y+=d;
            e-=2*dx;
        }
    }
}

void Rasterizer::drawPoint(const vertex_t& point, const color_t& color)
{
    int x=static_cast<int>(point.x());
    int y=static_cast<int>(point.y());

    if(x>=width || y>=height || x<0 || y<0)
        return;

    for(int i=-3; i<=3; i++)
        for(int j=-3; j<=3; j++)
            setPixel(x+i, y+j, color);
}

bool isInsideTriangle(int x, int y, const triangle_t& triangle)
{    
    const auto& vert=triangle.vertices;

    vec3f_t v[3];
    for(int i=0;i<3;i++)
        v[i] = {vert[i].x(), vert[i].y(), 1.0};

    vec3f_t f0,f1,f2;
    f0 = v[1].cross(v[0]);
    f1 = v[2].cross(v[1]);
    f2 = v[0].cross(v[2]);
    vec3f_t p(x,y,1.);

    if((p.dot(f0)*f0.dot(v[2])>0) && (p.dot(f1)*f1.dot(v[0])>0) && (p.dot(f2)*f2.dot(v[1])>0))
        return true;

    return false;
}

bool Rasterizer::isTriangleBackface(const triangle_t& triangle)
{
    const auto& v=triangle.vertices;
    vec3f_t v0(v[1].x()-v[0].x(), v[1].y()-v[0].y(), v[1].z()-v[0].z());
    vec3f_t v1(v[2].x()-v[0].x(), v[2].y()-v[0].y(), v[2].z()-v[0].z());
    float dot=v0.cross(v1).z();

    return dot<1e-2;
}
