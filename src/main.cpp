#include "gl.h"
#include "model.h"
#include <GLFW/glfw3.h>
#include "math_utils.h"


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

struct Camera {
    vec3 pos {0.0, -0.5, 0.0};

    Matrix4 to_matrix() {
        return Matrix4().trans(pos.x, pos.y, pos.z);
    }
} camera;

// Constants
double const MOVEMENT_SPEED {1.0};

/* Get the movement vector based on user input */
vec3 get_move_dir() {
    vec3 dir {};

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        dir.z = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        dir.z = -1;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        dir.x = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        dir.x += -1;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        dir.y = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        dir.y += -1;
    }

    return dir.normalize();
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

    // Update the camera on movement
    vec3 const dir = get_move_dir();
    if (!dir.is_zero()) {
        // Reset the fbo to not get blurry frames
        frame = 0;
        camera.pos += dir * MOVEMENT_SPEED * delta;
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
    camera.to_matrix().upload(program, "to_world");

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
    program = GL::create_program("vert_pass.glsl", "fragment.glsl");
    tex_program = GL::create_program("vert_pass.glsl", "frag_tex.glsl");
    fbo_current = GL::create_fbo();
    fbo_prev = GL::create_fbo();

    render_base = init_render_base();
    last_time = glfwGetTime();

    GL::run_loop(window, main_loop);
}
