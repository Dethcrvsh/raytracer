#include "gl.h"
#include <iostream>

GLFWwindow* window;
GLuint program;
GLuint VAO, VBO;

void main_loop() {
    // Clear the screen with a color (e.g., Cornflower Blue)
    glClearColor(0.39f, 0.58f, 0.93f, 1.0f); // RGBA
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);

    // Draw the triangle
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

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
        0.0f,  0.5f, 0.0f,  // Top
       -0.5f, -0.5f, 0.0f,  // Bottom left
        0.5f, -0.5f, 0.0f   // Bottom right
    };

    // Set up VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    GL::run_loop(window, main_loop);
}
