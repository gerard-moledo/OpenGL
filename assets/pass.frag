#version 330

out vec4 Frag_Color;

void main() {
    Frag_Color = vec4(gl_FragCoord.x / 800 * gl_FragCoord.y / 600, 0.0f, gl_FragCoord.y / 600, 1.0f);
}