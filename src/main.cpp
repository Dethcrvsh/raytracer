#include "gl.h"
#include "model.h"
#include <iostream>


GLFWwindow* window;
GLuint program;
GLuint VAO, VBO;
Model* m;
Model* m2;

void main_loop() {
    // Clear the screen with a color (e.g., Cornflower Blue)
    glClearColor(0.39f, 0.58f, 0.93f, 1.0f); // RGBA
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);
    glUniform2f(
        glGetUniformLocation(program, "resolution"),
        static_cast<float>(GL::WIDTH),
        static_cast<float>(GL::HEIGHT)
    );
    m->draw();

    // Swap front and back buffers
    glfwSwapBuffers(window);

    // Poll for and process events
    glfwPollEvents();
};

int main() {
    window = GL::init();
    program = GL::create_program("vertex.glsl", "fragment.glsl");

    // Vertex data
    float vertices[] = {
        -1.0f,  1.0f, 0.0f,  // Top
       -1.0f, -1.0f, 0.0f,  // Bottom left
        1.0f, -1.0f, 0.0f   // Bottom right
    };

    m = new Model({
        -1.0f,  1.0f, 0.0f,  // Top
       -1.0f, -1.0f, 0.0f,  // Bottom left
        1.0f, -1.0f, 0.0f,   // Bottom right
        -1.0f,  1.0f, 0.0f,  // Top
        1.0f, 1.0f, 0.0f,  // Top left
        1.0f, -1.0f, 0.0f   // Bottom right
    });

    GL::run_loop(window, main_loop);
}
