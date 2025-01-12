#version 330 core

in vec3 position;
out vec2 frag_coord;

void main() {
    gl_Position = vec4(position, 1.0);
    frag_coord = vec2(position);
}
