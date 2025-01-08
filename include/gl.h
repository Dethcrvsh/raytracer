#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <functional>
#include <string>


namespace GL {
    static int const WIDTH {800};
    static int const HEIGHT {600};

    GLFWwindow* init();
    void run_loop(GLFWwindow* const window, std::function<void()> const& callback);
    std::string read_file(std::string const& file_path);
    GLuint compile_shader(std::string const& source, GLenum const type);
    GLuint create_program(std::string const& vertex_path, std::string const& fragment_path);
};
