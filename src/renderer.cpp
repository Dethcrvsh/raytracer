#include "renderer.h"

namespace Renderer {
    void init() {
        // Initialize OpenGL
        state.window = GL::init();
        state.program = GL::create_program("vert_pass.glsl", "frag_trace.glsl");
        state.tex_program = GL::create_program("vert_pass.glsl", "frag_tex.glsl");
        state.fbo_current = GL::create_fbo();
        state.fbo_prev = GL::create_fbo();

        state.render_base = create_fullscreen_quad();
        state.last_time = glfwGetTime();

        state.uniforms.resolution = glGetUniformLocation(state.program, "resolution");
        state.uniforms.time = glGetUniformLocation(state.program, "time");
        state.uniforms.frame = glGetUniformLocation(state.program, "frame");
        state.uniforms.view_matrix = glGetUniformLocation(state.program, "view_matrix");
        state.uniforms.fov = glGetUniformLocation(state.program, "FOV");

        state.camera = Camera({0.0, 0.5, 0.0}, 70);

        // Bind the GLArray to the correct buffers
        state.spheres.bind(state.program, "sphere_buffer");
        state.spheres.bind_size(state.program, "SPHERES_NUM");

        state.spheres.push_back(
            Sphere(vec3(-1.0, 0.5, -2.0), 0.5,
                   Material().lambertian(vec3(1.0, 0.2, 1.0), 1.0)));

        state.spheres.push_back(
            Sphere(vec3(1.0, 0.5, -2.0), 0.5,
                   Material().metal(vec3(1.0, 1.0, 1.0), 1.0, 0.0))
        );

        state.spheres.push_back(
            Sphere(vec3(-0.5, 0.5, -6.0), 0.5,
                   Material().metal(vec3(1.0, 1.0, 1.0), 1.0, 0.0))
        );

        state.spheres.push_back(
            Sphere(vec3(-0.3, 0.1, -1.0), 0.1,
                   Material().lambertian(vec3(0.3, 0.7, 0.3), 1.0))
        );

        state.spheres.push_back(
            Sphere(vec3(-0.1, 0.1, -1.2), 0.1,
                   Material().lambertian(vec3(0.7, 0.3, 0.3), 1.0))
        );

        state.spheres.push_back(
            Sphere(vec3(0.3, 0.1, -1.1), 0.1,
                   Material().lambertian(vec3(0.3, 0.3, 0.7), 1.0))
        );

        state.spheres.push_back(
            Sphere(vec3(0.0, 0.25, -2.1), 0.25,
                   Material().metal(vec3(0.3, 0.3, 0.7), 1.0, 0.2))
        );

        GL::run_loop(state.window, update);
    }

    void update() {
        double now{glfwGetTime()};
        double delta{now - state.last_time};
        state.last_time = now;

        // Reset screen and viewport size
        glClearColor(0.39f, 0.58f, 0.93f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, GL::WIDTH, GL::HEIGHT);

        state.fbo_current.use();
        glUseProgram(state.program);

        state.spheres.upload();

        // Update the camera on movement
        if (state.camera.move(state.window, delta)) {
            // Reset the fbo to not get blurry frames
            state.frame = 0;
        }

        // Upload the previous fbo texture to blend with
        glBindTexture(GL_TEXTURE_2D, state.fbo_prev.texture);

        // Upload variables
        glUniform2f(state.uniforms.resolution, static_cast<GLfloat>(GL::WIDTH),
                    static_cast<GLfloat>(GL::HEIGHT));
        glUniform1f(state.uniforms.time, glfwGetTime());
        glUniform1i(state.uniforms.frame, state.frame);
        state.camera.to_matrix().upload(state.program, "view_matrix");
        glUniform1i(state.uniforms.fov, state.camera.fov);

        // Do the tracing of rays!
        state.render_base.draw(state.program, "in_position", "", "in_tex_coord");

        // Render the fbo
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(state.tex_program);
        glBindTexture(GL_TEXTURE_2D, state.fbo_current.texture);
        state.render_base.draw(state.tex_program, "in_position", "",
                               "in_tex_coord");

        // Swap front and back buffers
        glfwSwapBuffers(state.window);
        std::swap(state.fbo_current, state.fbo_prev);
        state.frame++;

        // Poll for and process events
        glfwPollEvents();
    }

    Model create_fullscreen_quad() {
        std::vector<GLfloat> const vertices = {
            -1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  0.0f, 1.0f, -1.0f, 0.0f};
        std::vector<GLfloat> const tex_coords = {
            0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0,
        };
        return {vertices, {}, tex_coords};
    }
}; // namespace Renderer
