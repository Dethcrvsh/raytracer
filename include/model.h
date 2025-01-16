#pragma once

#include "gl.h"
#include <vector>
#include <initializer_list>


struct Model {
    std::vector<float> vertices;
    GLuint vao;
    GLuint vbo;

    Model() = default;
    Model(std::initializer_list<float> const vertices);
    void draw() const;
};
