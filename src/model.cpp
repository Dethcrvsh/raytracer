#include "model.h"
#include "gl.h"
#include <iostream>

Model::Model(std::vector<GLfloat> const &verts,
             std::vector<GLfloat> const &norms,
             std::vector<GLfloat> const &texs)
: verts{verts}, norms{norms}, texs{texs} {
    // Set up VAO and VBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo_v);
    glGenBuffers(1, &vbo_n);
    glGenBuffers(1, &vbo_t);

    glBindVertexArray(vao);

    // Vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo_v);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(),
                 GL_STATIC_DRAW);

    // Normal data
    glBindBuffer(GL_ARRAY_BUFFER, vbo_n);
    glBufferData(GL_ARRAY_BUFFER, norms.size() * sizeof(GLfloat), norms.data(),
                 GL_STATIC_DRAW);

    // Texture data
    glBindBuffer(GL_ARRAY_BUFFER, vbo_t);
    glBufferData(GL_ARRAY_BUFFER, texs.size() * sizeof(GLfloat), texs.data(),
                 GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Model::draw(GLuint program, std::string const &vertex_var,
                 std::string const &normal_var,
                 std::string const &tex_var) const {
    GLint var;
    glBindVertexArray(vao);

    // Vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo_v);
    var = glGetAttribLocation(program, vertex_var.c_str());
    glVertexAttribPointer(var, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(var);

    // Normal data (not actually needed for this project but it felt weird to
    // leave out)
    if (normal_var != "") {
        glBindBuffer(GL_ARRAY_BUFFER, vbo_n);
        var = glGetAttribLocation(program, normal_var.c_str());
        glVertexAttribPointer(var, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(var);
    }

    // Texture data
    if (tex_var != "") {
        glBindBuffer(GL_ARRAY_BUFFER, vbo_t);
        var = glGetAttribLocation(program, tex_var.c_str());
        glVertexAttribPointer(var, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(var);
    }

    glDrawArrays(GL_TRIANGLES, 0, verts.size() / 3);
    glBindVertexArray(0);
}
