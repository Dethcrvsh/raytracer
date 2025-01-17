#include "gl.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace GL {

GLFWwindow* init() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* const window {glfwCreateWindow(800 * 1.5, 600 * 1.5, "OpenGL Test", nullptr, nullptr)};
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

void run_loop(GLFWwindow* const window, std::function<void()> const& callback) {
    while (!glfwWindowShouldClose(window)) {
        callback();
    }
}

std::string read_file(std::string const& file_path) {
    std::string const abs_fp {std::string{SHADER_DIR} + file_path};
    std::ifstream const ifs {abs_fp};

    if (!ifs.is_open()) {
        throw std::runtime_error("Failed to open shader file: " + abs_fp);
    }

    std::stringstream ss {};
    ss << ifs.rdbuf();
    return ss.str();
}

GLuint compile_shader(std::string const& source, GLenum const type) {
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

GLuint create_program(std::string const& vertex_path, std::string const& fragment_path) {
    // Read shader files
    std::string const vertex_code {read_file(vertex_path)};
    std::string const fragment_code {read_file(fragment_path)};

    // Compile Shaders
    GLuint vertex_shader {compile_shader(vertex_code, GL_VERTEX_SHADER)};
    GLuint fragment_shader {compile_shader(fragment_code, GL_FRAGMENT_SHADER)};

    // Create and link program
    GLuint const program {glCreateProgram()};
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    // Check linking status
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Program linking error: " << infoLog << std::endl;
        throw std::runtime_error("Program linking failed");
    }
    
    // Clean up shaders (no longer needed after linking)
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}

};
