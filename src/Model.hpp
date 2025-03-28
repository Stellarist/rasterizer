#pragma once

#include "global.hpp"
#include <string>
#include <vector>

#include "tiny_obj_loader.h"
#include "stb_image.h"

enum class TextureType {
	DIFFUSE,
	SPECULAR,
	BUMP
};

struct Texture {
	int width;
	int height;
	int nrChannels;
	std::string file_path;
	std::vector<color_t> data;
	TextureType          type;

	Texture(std::string file_path, TextureType type);
	color_t sample(float u, float v) const;
};

class Model {
private:
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::map<std::string, Texture*> textures;

	void readModel(const std::string& filepath);
	void readTextures(const std::string& filepath);

public:
	Model() = default;
	Model(const std::string& filepath);

	void setTextures(const std::map<std::string, Texture*>& textures);
	void addTextures(const std::string& filepath, TextureType type);
	auto getBoundingBox() -> std::pair<vec2f_t, vec2f_t>;
	auto getBoundingBoxCenter() -> vec2f_t;
	auto getBoundingBoxSize() -> vec2f_t;

	friend class Shader;
};
