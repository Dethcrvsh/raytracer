#include "gl.h"
#include <iostream>
#include <fstream>
#include <sstream>

GLFWwindow* GL::init() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* const window {glfwCreateWindow(800, 600, "OpenGL Test", nullptr, nullptr)};
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    // Set the swap interval (VSync)
    glfwSwapInterval(1);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW initialization failed: " << glewGetErrorString(err) << std::endl;
        return nullptr;
    }

    return window;
}

void GL::run_loop(GLFWwindow* const window, std::function<void()> const& callback) {
    while (!glfwWindowShouldClose(window)) {
        callback();
    }
}

std::string GL::read_file(std::string const& file_path) {
    std::string const abs_fp {std::string{SHADER_DIR} + file_path};
    std::ifstream const ifs {abs_fp};

    if (!ifs.is_open()) {
        throw std::runtime_error("Failed to open shader file: " + abs_fp);
    }

    std::stringstream ss {};
    ss << ifs.rdbuf();
    return ss.str();
}

GLuint GL::compile_shader(std::string const& source, GLenum const type) {
    GLuint const shader {glCreateShader(type)};
    char const* source_str {source.c_str()};

    glShaderSource(shader, 1, &source_str, nullptr);
    glCompileShader(shader);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
        throw std::runtime_error("Shader compilation failed");
    }

    return shader;
}
