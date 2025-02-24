#include "gl.h"
#include "model.h"
#include "tracer_objects.h"
#include <GLFW/glfw3.h>
#include "math_utils.h"
#include <cmath>

GLFWwindow* window;

// OpenGL variables
GLuint program;
GLuint tex_program;
GLuint VAO, VBO;
GLuint frame {};

// Frame buffer objects
GL::FBO fbo_current;
GL::FBO fbo_prev;

// Time counters
double last_time;

// Graphics objects
Model render_base;
GLArray<Sphere, 16> spheres {};

struct Camera {
    vec3 pos {0.0, 0.5, 0.0};
    GLfloat pitch;
    GLfloat yaw;

    Matrix4 to_matrix() {
        vec3 const up {0.0, 1.0, 0.0};
        Matrix4 rot = Matrix4().roty(-yaw) * Matrix4().rotx(-pitch);
        Matrix4 trans = Matrix4().trans(pos.x, pos.y, pos.z);
        return trans * rot;
    }
} camera;

// Constants
double const MOVEMENT_SPEED {1.0};
double const LOOK_SPEED {1.0};

/* Get the movement vector based on user input */
bool move_camera(Camera& cam, double delta) {
    vec3 const forward {std::sin(cam.yaw), 0.0, -std::cos(cam.yaw)};
    vec3 const right {std::cos(cam.yaw), 0.0, std::sin(cam.yaw)};
    vec3 const up {0.0, 1.0, 0.0};

    vec3 dir {};
    bool moved {};

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
        cam.pos += dir * MOVEMENT_SPEED * delta;
        moved = true;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        double new_yaw {cam.yaw - LOOK_SPEED * delta};
        cam.yaw = std::fmod(new_yaw, 2*3.1415);
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        double new_yaw {cam.yaw + LOOK_SPEED * delta};
        cam.yaw = std::fmod(new_yaw, 2*3.1415);
        moved = true;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        double new_pitch {cam.pitch - LOOK_SPEED * delta};
        cam.pitch = new_pitch;
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        double new_pitch {cam.pitch + LOOK_SPEED * delta};
        cam.pitch = new_pitch;
        moved = true;
    }

    return moved;
}

void main_loop() {
    double now {glfwGetTime()};
    double delta {now - last_time};
    last_time = now;

    // Reset screen and viewport size
    glClearColor(0.39f, 0.58f, 0.93f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, GL::WIDTH, GL::HEIGHT);

    fbo_current.use();
    glUseProgram(program);

    spheres.upload();

    // Update the camera on movement
    if (move_camera(camera, delta)) {
        // Reset the fbo to not get blurry frames
        frame = 0;
    }
    
    // Upload the previous fbo texture to blend with
    glBindTexture(GL_TEXTURE_2D, fbo_prev.texture);

    // Upload variables
    glUniform2f(
        glGetUniformLocation(program, "resolution"),
        static_cast<GLfloat>(GL::WIDTH),
        static_cast<GLfloat>(GL::HEIGHT)
    );
    glUniform1f(
        glGetUniformLocation(program, "time"),
        glfwGetTime()
    );
    glUniform1i(
        glGetUniformLocation(program, "frame"),
        frame
    );
    camera.to_matrix().upload(program, "view_matrix");

    // Do the tracing of rays!
    render_base.draw(program, "in_position", "", "in_tex_coord");

    // Render the fbo
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(tex_program);
    glBindTexture(GL_TEXTURE_2D, fbo_current.texture);
    render_base.draw(tex_program, "in_position", "", "in_tex_coord");

    // Swap front and back buffers
    glfwSwapBuffers(window);
    std::swap(fbo_current, fbo_prev);
    frame++;

    // Poll for and process events
    glfwPollEvents();
};

/* Initialize the square model that will be rendered to */
Model init_render_base() {
    std::vector<GLfloat> const vertices = {
        -1.0f,  1.0f, 0.0f, 
       -1.0f, -1.0f, 0.0f,  
        1.0f, -1.0f, 0.0f,  
       -1.0f,  1.0f, 0.0f, 
        1.0f, 1.0f, 0.0f, 
        1.0f, -1.0f, 0.0f
    };
    std::vector<GLfloat> const tex_coords = {
        0.0, 1.0,
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0,
    };
    return {vertices, {}, tex_coords};
}

int main() {
    // Initialize OpenGL
    window = GL::init();
    program = GL::create_program("vert_pass.glsl", "frag_trace.glsl");
    tex_program = GL::create_program("vert_pass.glsl", "frag_tex.glsl");
    fbo_current = GL::create_fbo();
    fbo_prev = GL::create_fbo();

    render_base = init_render_base();
    last_time = glfwGetTime();
    
    // Bind the GLArray to the correct buffers
    spheres.bind(program, "sphere_buffer");
    spheres.bind_size(program, "SPHERES_NUM");

    spheres.add({vec3(-1.0, 0.5, -2.0), 0.5, {{1.0, 0.2, 1.0}, 1.0, 0}});
    spheres.add({vec3(-0.5, 0.5, -6.0), 0.5, Material(vec3(1.0, 1.0, 1.0), 1, 0)});
    spheres.add({vec3(1.0, 0.5, -2.0), 0.5, Material(vec3(1.0, 1.0, 1.0), 0.0, 1.0)});
    spheres.add({vec3(-0.3, 0.1, -1.0), 0.1, Material(vec3(0.7, 0.3, 0.7), 0.0, 1.0)});
    spheres.add({vec3(0.15, 0.1, -1.3), 0.1, Material(vec3(0.8, 0.2, 0.1), 1.0, 0.0)});
    spheres.add({vec3(0.55, 0.1, -1.55), 0.1, Material(vec3(0.1, 0.1, 0.8), 1.0, 0.0)});

    GL::run_loop(window, main_loop);
}
