#include "Pipeline.hpp"

Camera* Pipeline::camera_ptr=nullptr;
Model* Pipeline::model_ptr=nullptr;
Rasterizer* Pipeline::rasterizer_ptr=nullptr;
Shader* Pipeline::shader_ptr=nullptr;

bool Pipeline::valid()
{
    return model_ptr!=nullptr;
}

void Pipeline::bind(Camera* camera_ptr)
{
    Pipeline::camera_ptr=camera_ptr;
    if(shader_ptr)
        shader_ptr->setViewPos(camera_ptr->getPosition());
}

void Pipeline::bind(Model* model_ptr)
{
    Pipeline::model_ptr=model_ptr;
    if(shader_ptr)
        shader_ptr->origin_model=*model_ptr;
}

void Pipeline::bind(Rasterizer* rasterizer_ptr)
{
    Pipeline::rasterizer_ptr=rasterizer_ptr;
}

void Pipeline::bind(Shader* shader_ptr)
{
    Pipeline::shader_ptr=shader_ptr;
}

void Pipeline::clear(color_t color)
{
    Pipeline::rasterizer_ptr->clear(color);
}

void Pipeline::render()
{
    if(!model_ptr || !shader_ptr)
        return;
    shader_ptr->setViewPos(camera_ptr->getPosition());
    shader_ptr->flush();
    shader_ptr->transform();
    shader_ptr->render();
}
