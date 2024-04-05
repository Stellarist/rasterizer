#pragma once

#include <string>

#include "global.hpp"
#include "stb_image.h"

enum TextureType {
    DIFFUSE,
    SPECULAR,
    BUMP
};

class Texture{
private:
    int                  width;
    int                  height;
    int                  nrChannels;
    std::string          file_path;
    std::vector<color_t> data;
    TextureType          type;

public:
    Texture(std::string file_path, TextureType type);

    int getWidth() const      {return width;}
    int getHeight() const     {return height;}
    int getNrChannels() const {return nrChannels;}

    std::string          getFilePath() const        {return file_path;}
    std::vector<color_t> getTextureData() const     {return data;}
    TextureType          getTextureType() const     {return type;}

    color_t sample(float u, float v) const;
};
