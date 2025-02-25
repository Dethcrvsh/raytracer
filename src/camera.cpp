#include "camera.h"
#include <cmath>

Camera::Camera(vec3 const& pos) : pos{pos} {};

Matrix4 Camera::to_matrix() const {
    vec3 const up{0.0, 1.0, 0.0};
    Matrix4 rot = Matrix4().roty(-yaw) * Matrix4().rotx(-pitch);
    Matrix4 trans = Matrix4().trans(pos.x, pos.y, pos.z);
    return trans * rot;
}

bool Camera::move(GLFWwindow* const window, double const delta) {
    vec3 const forward{std::sin(yaw), 0.0, -std::cos(yaw)};
    vec3 const right{std::cos(yaw), 0.0, std::sin(yaw)};
    vec3 const up{0.0, 1.0, 0.0};

    vec3 dir{};
    bool moved{};

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        dir += forward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        dir -= forward;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        dir += right;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        dir -= right;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        dir += up;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        dir -= up;

    if (!dir.is_zero()) {
        dir = dir.normalize();
        pos += dir * MOVEMENT_SPEED * delta;
        moved = true;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        double new_yaw{yaw - LOOK_SPEED * delta};
        yaw = std::fmod(new_yaw, 2 * 3.1415);
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        double new_yaw{yaw + LOOK_SPEED * delta};
        yaw = std::fmod(new_yaw, 2 * 3.1415);
        moved = true;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        double new_pitch{pitch - LOOK_SPEED * delta};
        pitch = new_pitch;
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        double new_pitch{pitch + LOOK_SPEED * delta};
        pitch = new_pitch;
        moved = true;
    }

    return moved;
}
