#version 330

out vec4 Frag_Color;
in vec4 out_color;

void main() {
    Frag_Color = out_color;
}