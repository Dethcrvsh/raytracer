#pragma once
#include "camera.h"
#include "gl.h"
#include "model.h"
#include "tracer_objects.h"

namespace Renderer {
    struct State {
        GLFWwindow* window;

        // OpenGL variables
        GLuint program;
        GLuint tex_program;
        GLuint VAO, VBO;
        GLuint frame;
        
        struct ShaderUniforms {
            GLint resolution;
            GLint time;
            GLint frame;
            GLint view_matrix;
            GLint fov;
        } uniforms;

        // Frame buffer objects
        GL::FBO fbo_current;
        GL::FBO fbo_prev;

        // Time counters
        double last_time;

        // Graphics objects
        Model render_base;
        GLArray<Sphere, 16> spheres;

        Camera camera;
    };

    static State state;

    void init();
    void update();
    Model create_fullscreen_quad();
};
