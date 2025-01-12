#include "model.h"


Model::Model(std::initializer_list<float> const vert) : vertices {vert} {
    // Set up VAO and VBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Model::draw() const {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
    glBindVertexArray(0);
}
