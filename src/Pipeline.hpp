#pragma once

#include <vector>

#include "Camera.hpp"
#include "Model.hpp"
#include "Rasterizer.hpp"
#include "Shader.hpp"

enum class CallbackType {
	INIT,
	TICK,
	DESTROY
};

class Pipeline {
private:
	int         width, height;
	std::string model_path, texture_path;

	std::array<std::vector<std::function<void()>>, 3> callback_queue;

	Pipeline();
	~Pipeline();

	Pipeline(const Pipeline&) = delete;
	Pipeline& operator=(const Pipeline&) = delete;
	Pipeline(Pipeline&&) = delete;
	Pipeline& operator=(Pipeline&&) = delete;

public:
	static Pipeline instance;

	Camera*     camera;
	Model*      model;
	Rasterizer* rasterizer;
	Shader*     shader;

	static void init();
	static void tick();
	static void destroy();
	static void clear(color_t color = color_t{1.f, 1.f, 1.f});
	static void render();
	static void flush();

	static int getWidth() { return instance.width; }
	static int getHeight() { return instance.height; }

	template <typename T, typename... Args>
	static void reg(CallbackType type, T&& func, Args&&... args)
	{
		instance.callback_queue[static_cast<int>(type)].emplace_back([=]() mutable {
			std::invoke(std::forward<T>(func), std::forward<Args>(args)...);
		});
	}

	friend class Window;
};