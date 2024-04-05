#define STB_IMAGE_IMPLEMENTATION

#include "Texture.hpp"

#include <string>
#include <iostream>

Texture::Texture(std::string file_path, TextureType type)
{
    this->file_path = file_path;
    this->type = type;
    auto image=stbi_load(file_path.c_str(), &this->width, &this->height, &this->nrChannels, 0); 

    if(image==nullptr){
        std::cerr<<"Failed to load texture "<<file_path<<std::endl;
        exit(1);
    }

    data.resize(this->width*this->height);
    for(int i=0; i<this->width*this->height; i++)
        data[i]=color_t(
            image[i*nrChannels+0]/255.0f,
            image[i*nrChannels+1]/255.0f,
            image[i*nrChannels+2]/255.0f
        );

    stbi_image_free(image);
}

color_t Texture::sample(float u, float v) const 
{
    int u_img=u*width;
    int v_img=v*height;
    int index=u_img*width+v_img;
    if(index<0)
        index=0;

    return data[index%data.size()];
}
