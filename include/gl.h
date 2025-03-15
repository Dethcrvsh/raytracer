#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <functional>
#include <string>
#include <cassert>


namespace GL {
    static int const WIDTH {1280};
    static int const HEIGHT {960};
    
    typedef struct FBO {
        GLuint fbo;
        GLuint texture;
        
        void use() const;
    } FBO;

    GLFWwindow* init();
    void run_loop(GLFWwindow* const window, std::function<void()> const& callback);
    std::string read_file(std::string const& file_path);
    GLuint compile_shader(std::string const& source, GLenum const type);
    GLuint create_program(std::string const& vertex_code, std::string const& fragment_code);
    GLuint create_program_from_file(std::string const& vertex_path, std::string const& fragment_path);
    FBO create_fbo();
    GLuint get_binding_point();
};


template <typename T, size_t MAX_SIZE>
class GLArray {
public:
    void bind(GLuint program, std::string const& block_name) {
        this->program = program;
        this->block_name = block_name;
        // WARN: This is absolutely not thread safe
        this->binding_point = GL::get_binding_point();

        // Create a uniform buffer object
        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);

        // Allocate memory in the buffer
        glBufferData(
            GL_UNIFORM_BUFFER, sizeof(T) * MAX_SIZE, nullptr, GL_STATIC_DRAW
        );

        GLuint const block_index = glGetUniformBlockIndex(program, block_name.c_str());

        // Bind the buffer to a binding point
        glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ubo);

        glUniformBlockBinding(program, block_index, binding_point);
    }

    /* Bind to a variable that will track the size of the array */
    void bind_size(GLuint program, std::string const& size_name) {
        size_var = glGetUniformLocation(program, size_name.c_str());
    }

    void upload() const {
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, vector.size() * sizeof(T), vector.data());

        if (size_var != -1) {
            glUniform1i(size_var, vector.size());
        }
    }

    T& at(size_t index) {
        return vector.at(index);
    }

    T const& at(size_t index) const {
        return vector.at(index);
    }

    T& operator[](size_t index) {
        assert(index < MAX_SIZE);
        return vector[index];
    }

    T const& operator[](size_t index) const {
        assert(index < MAX_SIZE);
        return vector[index];
    }

    bool empty() const {
        return vector.empty();
    }

    size_t size() const {
        return vector.size();
    }

    size_t max_size() const {
        return MAX_SIZE;
    }

    void clear() {
        vector.clear();
    }

    void erase(size_t index) {
        vector.erase(vector.begin() + index);
    }

    void push_back(T const& value) {
        assert(vector.size() < MAX_SIZE);
        vector.push_back(value);
    } 

    void push_back(T&& value) {
        vector.push_back(std::forward<T>(value));
    }

    void pop_back() {
        vector.pop_back();
    }

private:
    GLuint ubo;
    GLuint program;
    GLuint binding_point;
    GLint size_var {-1};
    std::string block_name;
    std::vector<T> vector {};
};
