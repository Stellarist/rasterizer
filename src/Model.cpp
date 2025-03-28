#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include "Model.hpp"

#include <cstddef>
#include <iostream>

Texture::Texture(std::string file_path, TextureType type)
{
	this->file_path = file_path;
	this->type = type;
	auto image = stbi_load(file_path.c_str(), &this->width, &this->height, &this->nrChannels, 0);
	stbi_set_flip_vertically_on_load(true);

	if (image == nullptr) {
		std::cerr << "Failed to load texture " << file_path << std::endl;
		exit(1);
	}

	data.resize(this->width * this->height);
	for (int i = 0; i < this->width * this->height; i++)
		data[i] = color_t(
			image[i * nrChannels + 0] / 255.0f,
			image[i * nrChannels + 1] / 255.0f,
			image[i * nrChannels + 2] / 255.0f
		);

	stbi_image_free(image);
}

color_t Texture::sample(float u, float v) const
{
	int u_img = u * width;
	int v_img = v * height;
	int index = u_img * width + v_img;
	if (index < 0)
		index = 0;

	return data[index % data.size()];
}

Model::Model(const std::string& filepath)
{
	readModel(filepath);
	readTextures(filepath);
}

void Model::readModel(const std::string& filepath)
{
	// get file directory and name
	size_t file_pos = filepath.find_last_of('/');
	std::string file_dir = filepath.substr(0, file_pos + 1);
	std::string file_name = filepath.substr(file_pos + 1);

	// load obj file
	tinyobj::ObjReader reader;
	tinyobj::ObjReaderConfig reader_config;
	reader_config.triangulate = true;
	reader_config.mtl_search_path = file_dir;

	if (!reader.ParseFromFile(file_dir + file_name, reader_config)) {
		if (!reader.Error().empty()) {
			std::cerr << "TinyObjReader1: " << reader.Error() << std::endl;
		}
		exit(1);
	}
	if (!reader.Warning().empty()) {
		std::cerr << "TinyObjReader2: " << reader.Error() << std::endl;
	}

	// read properties
	attrib = reader.GetAttrib();
	shapes = reader.GetShapes();
	materials = reader.GetMaterials();
}

void Model::readTextures(const std::string& filepath)
{
	size_t file_pos = filepath.find_last_of('/');
	std::string file_dir = filepath.substr(0, file_pos + 1);

	for (auto& material : materials) {
		if (!material.diffuse_texname.empty())
			textures[material.diffuse_texname] = new Texture(file_dir + material.diffuse_texname, TextureType::DIFFUSE);

		if (!material.specular_texname.empty())
			textures[material.specular_texname] = new Texture(file_dir + material.specular_texname, TextureType::SPECULAR);

		if (!material.bump_texname.empty())
			textures[material.bump_texname] = new Texture(file_dir + material.bump_texname, TextureType::BUMP);
	}
}

void Model::setTextures(const std::map<std::string, Texture*>& textures)
{
	this->textures = textures;
}

void Model::addTextures(const std::string& filepath, TextureType type)
{
	if (filepath.empty())
		return;
	size_t file_pos = filepath.find_last_of('/');
	std::string file_name = filepath.substr(file_pos + 1);
	textures[file_name] = new Texture(filepath, type);
}

auto Model::getBoundingBox() -> std::pair<vec2f_t, vec2f_t>
{
	vec2f_t min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	vec2f_t max(std::numeric_limits<float>::min(), std::numeric_limits<float>::min());

	for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
		min.x() = std::min(min.x(), attrib.vertices[i + 0]);
		min.y() = std::min(min.y(), attrib.vertices[i + 1]);
		max.x() = std::max(max.x(), attrib.vertices[i + 0]);
		max.y() = std::max(max.y(), attrib.vertices[i + 1]);
	}

	return std::make_pair(min, max);
}

auto Model::getBoundingBoxCenter() -> vec2f_t
{
	auto [min, max] = getBoundingBox();
	return (min + max) / 2;
}

auto Model::getBoundingBoxSize() -> vec2f_t
{
	auto [min, max] = getBoundingBox();
	return max - min;
}
