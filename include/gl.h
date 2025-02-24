#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <functional>
#include <string>
#include <cassert>


namespace GL {
    static int const WIDTH {640 * 2};
    static int const HEIGHT {480 * 2};
    
    typedef struct FBO {
        GLuint fbo;
        GLuint texture;
        
        void use() const;
    } FBO;

    GLFWwindow* init();
    void run_loop(GLFWwindow* const window, std::function<void()> const& callback);
    std::string read_file(std::string const& file_path);
    GLuint compile_shader(std::string const& source, GLenum const type);
    GLuint create_program(std::string const& vertex_path, std::string const& fragment_path);
    FBO create_fbo();
};


template <typename T, size_t MAX_SIZE>
class GLArray {
public:
    void bind(GLuint program, std::string const& block_name) {
        this->program = program;
        this->block_name = block_name;

        // Create a uniform buffer object
        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);

        // Allocate memory in the buffer
        glBufferData(
            GL_UNIFORM_BUFFER, sizeof(T) * MAX_SIZE, nullptr, GL_STATIC_DRAW
        );

        GLuint const block_index = glGetUniformBlockIndex(program, block_name.c_str());

        // Bind the buffer to a binding point
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);

        glUniformBlockBinding(program, block_index, 0);
    }

    /* Bind to a variable that will track the size of the array */
    void bind_size(GLuint program, std::string const& size_name) {
        size_var = glGetUniformLocation(program, size_name.c_str());
    }

    T operator[](size_t index) const {
        return array[index];
    }

    T& operator[](size_t index) {
        return array[index];
    }

    void add(T const& elem) {
        assert(array.size() < MAX_SIZE);
        array.push_back(elem);
    }

    void erase(size_t index) {
        array.erase(array.begin() + index);
    }

    void upload() const {
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, array.size() * sizeof(T), array.data());

        if (size_var != -1) {
            glUniform1i(size_var, array.size());
        }
    }

private:
    GLuint ubo;
    GLuint program;
    GLint size_var {-1};
    std::string block_name;
    std::vector<T> array {};
};
