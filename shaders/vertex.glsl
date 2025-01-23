#version 330 core

in vec3 position;
in vec3 normal;
in vec2 tex;
out vec2 frag_coord;

void main() {
    gl_Position = vec4(position, 1.0);
    frag_coord = vec2(position);
}
