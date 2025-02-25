#pragma once
#include "math_utils.h"

struct Camera {
    double static constexpr MOVEMENT_SPEED{1.0};
    double static constexpr LOOK_SPEED{1.0};

    vec3 pos;
    GLfloat pitch;
    GLfloat yaw;

    Camera() = default;
    Camera(vec3 const& pos);
    Matrix4 to_matrix() const;
    bool move(GLFWwindow* const window, double const delta);
};
