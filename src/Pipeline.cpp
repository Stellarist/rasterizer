#include "Pipeline.hpp"
#include "global.hpp"

#include <yaml-cpp/yaml.h>

Pipeline Pipeline::instance;

Pipeline::Pipeline()
{
	std::string config_path = PROJECT_PATH "/config.yaml";
	YAML::Node config = YAML::LoadFile(config_path);
	width = config["window_width"].as<int>();
	height = config["window_height"].as<int>();
	model_path = PROJECT_PATH "/" + config["model_path"].as<std::string>();
	texture_path = PROJECT_PATH "/" + config["texture_path"].as<std::string>();

	model = new Model(model_path);
	model->addTextures(texture_path, TextureType::DIFFUSE);

	camera = new Camera(direct_t{ width / 2.f, height / 2.f, 3.0f });
	rasterizer = new Rasterizer(width, height);
	shader = new Shader();
}

Pipeline::~Pipeline()
{
	delete model;
	delete camera;
	delete rasterizer;
	delete shader;
}

void Pipeline::init()
{
	for (auto& func : instance.callback_queue[static_cast<int>(CallbackType::INIT)]) {
		func();
	}
}

void Pipeline::tick()
{
	for (auto& func : instance.callback_queue[static_cast<int>(CallbackType::TICK)]) {
		func();
	}
}

void Pipeline::destroy()
{
	for (auto& func : instance.callback_queue[static_cast<int>(CallbackType::DESTROY)]) {
		func();
	}
}

void Pipeline::clear(color_t color)
{
	instance.rasterizer->clear(color);
}

void Pipeline::render()
{
	if (!instance.model || !instance.shader)
		return;
	instance.shader->setViewPos(instance.camera->getPosition());
	instance.shader->flush();
	instance.shader->transform();
	instance.shader->render();
}
