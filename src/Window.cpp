#include "Window.hpp"

#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <yaml-cpp/yaml.h>

#include "global.hpp"
#include "Pipeline.hpp"

Window::Window()
{
    initConfig();
    initWindow();
    initScreen();
    initRender();
}

Window::~Window()
{
    delete shader;
    delete rasterizer;
    delete model;
    delete camera;

    release();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::initRender()
{
    model=new Model(model_path);
    model->addTextures(texture_path, TextureType::DIFFUSE);

    direct_t view_pos(width/2, height/2, 3.0f);
    camera=new Camera(view_pos);
    rasterizer=new Rasterizer(width, height);
    shader=new Shader();

    auto mat=shader->getModelMat();
    auto center=model->getBoundingBoxCenter();
    auto size=model->getBoundingBoxSize();
    auto scale=0.75f*std::min(width/size.x(), height/size.y());
    mat=Geometry::translate(mat, direct_t(width/2-center.x(), height/2-center.y(), 0.f));
    mat=Geometry::scale(mat, direct_t(scale, -scale, scale));
    shader->setModelMat(mat);

    Pipeline::bind(camera);
    Pipeline::bind(rasterizer);
    Pipeline::bind(shader);
    Pipeline::bind(model);
}

void Window::transformRender()
{
    auto mat=shader->getModelMat();
    mat=Geometry::rotate(mat, glfwGetTime(), direct_t(0.f, 1.f, 0.f));
    shader->setModelMat(mat);
    // shader->setViewMat(camera->getViewMat());
    // shader->setProjectionMat(camera->getProjectionMat());
}

void Window::run()
{
    while(!glfwWindowShouldClose(window)){
        double start=glfwGetTime();

        Pipeline::clear({1.f, 1.f, 1.f});
        Pipeline::render();

        transformRender();
        processInput();

        glUseProgram(window_shader);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, rasterizer->getFramebufferData());
        glBindVertexArray(this->vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();

        double end=glfwGetTime();
        std::cerr<<1.f/(end-start)<<"fps"<<std::endl;
    }
}
#include <iostream>
void Window::initConfig()
{
    std::string config_path=PROJECT_PATH "/config/config.yaml";
    YAML::Node config=YAML::LoadFile(config_path);
    width=config["window_width"].as<int>();
    height=config["window_height"].as<int>();
    vert_path=PROJECT_PATH "/" + config["vert_path"].as<std::string>();
    frag_path=PROJECT_PATH "/" + config["frag_path"].as<std::string>();
    model_path=PROJECT_PATH "/" + config["model_path"].as<std::string>();
    texture_path=PROJECT_PATH "/" + config["texture_path"].as<std::string>();
}

void Window::initWindow()
{
    glfwInit();
    window=glfwCreateWindow(width, height, "Rasterizer", nullptr, nullptr);
    if(window==nullptr){
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
    glViewport(0, 0, width, height);

    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(this->window, frameBufferSizeCallback);
}

void Window::initScreen()
{
    createGlShader(vert_path.c_str(), frag_path.c_str());
    float vertices[]={
        -1.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
    };

    // set vao and vbo
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // set texture
    glGenTextures(1, &window_texture);
    glBindTexture(GL_TEXTURE_2D, window_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUseProgram(window_shader);
    glUniform1i(glGetUniformLocation(window_shader, "texture0"), 0);
}

void Window::release()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(window_shader);
}

void Window::processInput()
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void Window::frameBufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Window::createGlShader(const char* vertex_path, const char* fragment_path)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertex_source;
    std::string fragment_source;
    std::ifstream vertex_file;
    std::ifstream fragment_file;
    vertex_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragment_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try{
        vertex_file.open(vertex_path);
        fragment_file.open(fragment_path);
        std::stringstream vertex_stream, fragment_stream;
        vertex_stream<<vertex_file.rdbuf();
        fragment_stream<<fragment_file.rdbuf();
        vertex_file.close();
        fragment_file.close();
        vertex_source=vertex_stream.str();
        fragment_source=fragment_stream.str();
    }catch(std::ifstream::failure e){
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    const char* vertex_code=vertex_source.c_str();
    const char* fragment_code=fragment_source.c_str();

    // 2. compile shaders
    unsigned int vertex, fragment;
    vertex=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_code, nullptr);    
    glCompileShader(vertex);
    checkGlShader(vertex, "VERTEX");

    fragment=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_code, nullptr);
    glCompileShader(fragment);
    checkGlShader(fragment, "FRAGMENT");

    window_shader=glCreateProgram();
    glAttachShader(window_shader, vertex);
    glAttachShader(window_shader, fragment);
    glLinkProgram(window_shader);
    checkGlShader(window_shader, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Window::checkGlShader(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if(type != "PROGRAM"){
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog 
                << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }else{
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if(!success){
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog 
                << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
