#pragma once

#include <gl.h>
#include <GLFW/glfw3.h>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <ostream>

using std::size_t;

// I started making a super general matrix class with templates
// But I don't actually need it
// So that was unnecessary

struct Matrix4 {
    static size_t const N {4};
    GLfloat m[N*N];

    Matrix4() = default;
    Matrix4(std::initializer_list<GLfloat> const& vals);

    Matrix4& trans(GLfloat x, GLfloat y, GLfloat z);
    Matrix4& rotx(GLfloat angle);
    Matrix4& roty(GLfloat angle);
    Matrix4& rotz(GLfloat angle);

    Matrix4 operator+(Matrix4 const& other) const;
    Matrix4 operator*(Matrix4 const& other) const;

    void upload(GLuint program, std::string const& var) const;

    friend std::ostream &operator<<(std::ostream &os, Matrix4 const &mat);
};

struct vec3 {
    GLfloat x;
    GLfloat y;
    GLfloat z;

    vec3 operator+(vec3 const& other) const;
    vec3 operator-(vec3 const& other) const;
    vec3 operator*(GLfloat f) const;
    vec3 operator*(vec3 const& other) const;
    vec3 operator/(GLfloat f) const;
    bool operator==(vec3 const& other) const;
    vec3& operator+=(vec3 const& other);

    vec3& normalize();

    bool is_zero() const;

    friend std::ostream &operator<<(std::ostream &os, vec3 const &v);
};
