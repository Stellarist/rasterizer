#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Camera.hpp"
#include "Rasterizer.hpp"
#include "Shader.hpp"

class Window{
private:
    int width, height;
    unsigned int vao, vbo, window_texture, window_shader;
    std::string vert_path, frag_path, model_path, texture_path;

    GLFWwindow* window;
    Camera*     camera;
    Model*      model;
    Rasterizer* rasterizer;
    Shader*     shader;

    void release();
    void processInput();
    void createGlShader(const char* vertex_path, const char* fragment_path);
    void checkGlShader(unsigned int shader, std::string type);

    static void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
    static void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

public:
    Window();
    ~Window();

    int getWidth() {return width;}
    int getHeight() {return height;}

    void initWindow();
    void initConfig();
    void initScreen();
    void initRender();
    void transformRender();
    void run();
};
