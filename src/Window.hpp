#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Pipeline.hpp"

class Window {
private:
	int& width = Pipeline::instance.width;
	int& height = Pipeline::instance.height;

	unsigned int pbo, texture;

	GLFWwindow* window;

	Window();
	~Window();

public:
	void run();
	void processInput();

	static Window& instance();
};

inline Window& Window::instance()
{
	static Window instance;
	return instance;
}
