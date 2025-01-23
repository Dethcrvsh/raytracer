#pragma once

#include "gl.h"
#include <vector>

struct Model {
    std::vector<GLfloat> verts;
    std::vector<GLfloat> norms;
    std::vector<GLfloat> texs;

    GLuint vao;
    GLuint vbo_v;
    GLuint vbo_n;
    GLuint vbo_t;

    Model() = default;
    Model(std::vector<GLfloat> const &verts, std::vector<GLfloat> const &norms,
          std::vector<GLfloat> const &texs);
    void draw(GLuint program, std::string const &vertex_var, std::string const &normal_var,
              std::string const &tex_var) const;
};
