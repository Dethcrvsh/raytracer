#include "gl.h"
#include <iostream>

GLFWwindow* window;

void main_loop() {
    // Clear the screen with a color (e.g., Cornflower Blue)
    glClearColor(0.39f, 0.58f, 0.93f, 1.0f); // RGBA
    glClear(GL_COLOR_BUFFER_BIT);

    // Swap front and back buffers
    glfwSwapBuffers(window);

    // Poll for and process events
    glfwPollEvents();
};

int main() {
    window = GL::init();
    std::string test = GL::read_file("vertex.glsl");
    GL::compile_shader(test, GL_VERTEX_SHADER);
    GL::run_loop(window, main_loop);
}
