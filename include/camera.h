#pragma once
#include "math_utils.h"

struct Camera {
    double static constexpr MOVEMENT_SPEED{1.0};
    double static constexpr LOOK_SPEED{1.0};
    int static constexpr FOV_MIN{5};
    int static constexpr FOV_MAX{150};

    vec3 pos;
    GLfloat pitch;
    GLfloat yaw;
    GLint fov;

    Camera() = default;
    Camera(vec3 const& pos, GLint fov);
    Matrix4 to_matrix() const;
    bool move(GLFWwindow* const window, double const delta);
};
