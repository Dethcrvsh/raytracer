#include "gl.h"
#include "model.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <ostream>


GLFWwindow* window;
GLuint program;
GLuint tex_program;
GLuint VAO, VBO;
Model m;
GL::FBO fbo;

void main_loop() {
    // Clear the screen with a color (e.g., Cornflower Blue)
    glClearColor(0.39f, 0.58f, 0.93f, 1.0f); // RGBA
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, GL::WIDTH, GL::HEIGHT);

    fbo.use();

    glUseProgram(program);
    glUniform2f(
        glGetUniformLocation(program, "resolution"),
        static_cast<float>(GL::WIDTH),
        static_cast<float>(GL::HEIGHT)
    );
    glUniform1f(
        glGetUniformLocation(program, "time"),
        glfwGetTime()
    );
    m.draw(program, "in_position", "", "");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(tex_program);

    glBindTexture(GL_TEXTURE_2D, fbo.texture);
    m.draw(tex_program, "in_position", "", "in_tex_coord");

    // Swap front and back buffers
    glfwSwapBuffers(window);

    // Poll for and process events
    glfwPollEvents();
};

int main() {
    window = GL::init();
    program = GL::create_program("vert_pass.glsl", "fragment.glsl");
    tex_program = GL::create_program("vert_pass.glsl", "frag_tex.glsl");

    fbo = GL::create_fbo();

    // Vertex data
    std::vector<GLfloat> const vertices = {
        -1.0f,  1.0f, 0.0f, 
       -1.0f, -1.0f, 0.0f,  
        1.0f, -1.0f, 0.0f,  
       -1.0f,  1.0f, 0.0f, 
        1.0f, 1.0f, 0.0f, 
        1.0f, -1.0f, 0.0f
    };

    std::vector<GLfloat> const tex_coords = {
        0.0, 1.0,
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0,
    };

    m = Model(vertices, {}, tex_coords);

    GL::run_loop(window, main_loop);
}
