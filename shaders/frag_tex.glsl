#version 330 core

in vec2 frag_coord;
in vec2 tex_coord;

out vec4 out_color;

uniform sampler2D tex;

void main() {
    out_color = texture(tex, tex_coord);
}
