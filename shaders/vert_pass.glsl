#version 330 core

in vec3 in_position;
in vec2 in_tex_coord;

out vec2 frag_coord;
out vec2 tex_coord;

void main() {
    gl_Position = vec4(in_position, 1.0);
    frag_coord = vec2(in_position);
    tex_coord = in_tex_coord;
}
