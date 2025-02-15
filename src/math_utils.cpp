#include "math_utils.h"
#include <algorithm>
#include <cmath>


Matrix4::Matrix4(std::initializer_list<GLfloat> const& vals) {
    assert(vals.size() == N*N);
    std::copy(std::begin(vals), std::end(vals), std::begin(m));
}

Matrix4& Matrix4::trans(GLfloat x, GLfloat y, GLfloat z) {
    m[0] = 1;
    m[3] = x;
    m[N + 1] = 1;
    m[N + 3] = y;
    m[N*2 + 2] = 1;
    m[N*2 + 3] = z;
    m[N*3 + 3] = 1;
    return *this;
}

Matrix4& Matrix4::rotx(GLfloat angle) {
    m[0] = 1;
    m[N + 1] = std::cos(angle);
    m[N + 2] = -std::sin(angle);
    m[2*N + 1] = std::sin(angle);
    m[2*N + 2] = std::cos(angle);
    m[3*N + 3] = 1;
    return *this;
}

Matrix4& Matrix4::roty(GLfloat angle) {
    m[0] = std::cos(angle);
    m[2] = std::sin(angle);
    m[N + 1] = 1;
    m[2*N] = -std::sin(angle);
    m[2*N + 2] = std::cos(angle);
    m[3*N + 3] = 1;
    return *this;
}

Matrix4& Matrix4::rotz(GLfloat angle) {
    m[0] = std::cos(angle);
    m[1] = -std::sin(angle);
    m[N] = std::sin(angle);
    m[N + 1] = std::cos(angle);
    m[2*N + 2] = 1;
    m[3*N + 3] = 1;
    return *this;
}

Matrix4& Matrix4::look_at(vec3 const& pos, vec3 const& look, vec3 const& up) {
    vec3 const f {look - pos}; 
    vec3 const r {up.cross(f) / up.cross(f).length()};
    vec3 const u {f.cross(r)};

    Matrix4 const rot {
        r.x, u.x, f.x, 0.0,
        r.y, u.y, f.y, 0.0,
        r.z, u.z, f.z, 0.0,
        0.0, 0.0, 0.0, 1.0,
    };
    Matrix4 const trans {
        Matrix4().trans(-pos.x, -pos.y, -pos.z)
    };

    *this = rot * trans;
    return *this;
}

Matrix4 Matrix4::operator+(Matrix4 const& other) const {
    Matrix4 res {};
    std::transform(
        std::begin(m),
        std::end(m),
        std::begin(other.m),
        std::begin(res.m),
        [] (GLfloat a, GLfloat b) {return a + b;}
    );

    return res;
}

Matrix4 Matrix4::operator*(Matrix4 const& other) const {
    Matrix4 res{};
    for (size_t x = 0; x < N; x++) {
        for (size_t y = 0; y < N; y++) {
            res.m[x*N + y] = 
                m[x*N] * other.m[y] +
                m[x*N + 1] * other.m[y + N] + 
                m[x*N + 2] * other.m[y + 2*N] + 
                m[x*N + 3] * other.m[y + 3*N];
        }
    }

    return res;
}

void Matrix4::upload(GLuint program, std::string const& var) const {
    glUniformMatrix4fv(
        glGetUniformLocation(program, var.c_str()),
        1, GL_FALSE, &m[0]
    );
}

std::ostream &operator<<(std::ostream &os, Matrix4 const &mat) {
    os << "[\n";
    for (int y = 0; y < mat.N; y++) {
        os << "  ";
        for (int x = 0; x < mat.N; x++) {
            os << mat.m[x + y * mat.N] << ", ";
        }
        os << "\n";
    }
    os << "]\n";
    return os;
}

vec3 vec3::operator+(vec3 const& other) const {
    return {x + other.x, y + other.y, z + other.z};
}

vec3 vec3::operator-(vec3 const& other) const {
    return {x - other.x, y - other.y, z - other.z};
}

vec3 vec3::operator*(GLfloat f) const {
    return {x * f, y * f, z * f};
}

vec3 vec3::operator*(vec3 const& other) const {
    return {x * other.x, y * other.y, z * other.z};
}

vec3 vec3::operator/(GLfloat f) const {
    return {x / f, y / f, z / f};
}

GLfloat vec3::dot(vec3 const& other) const {
    return x*other.x + y*other.y + z*other.z;
}

vec3 vec3::cross(vec3 const& other) const {
    return {
        y*other.z - z*other.y,
        z*other.x - x*other.z,
        x*other.y - y*other.x
    };
}

bool vec3::operator==(vec3 const& other) const {
    return x == other.x && y == other.y && z == other.z;
}

vec3& vec3::operator+=(vec3 const& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

vec3& vec3::operator-=(vec3 const& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

bool vec3::is_zero() const {
    vec3 const zero {0.0, 0.0, 0.0};
    return *this == zero;
}

GLfloat vec3::length() const {
    return std::sqrt(x*x + y*y + z*z);
}

vec3 vec3::normalize() {
    double l = length();
    // Only normalize if not 0
    if (l < 1e-6) {
        std::cerr << "Don't normalize a zero vector silly :3" << std::endl;
        return *this;
    }
    return *this / l;
}

std::ostream &operator<<(std::ostream &os, vec3 const &v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}


