#version 330 core

in vec3 vertex_color;
out vec4 color;

void main() {
    color = vec4(vertex_color * 2 + 0.5, 1.0);
}
