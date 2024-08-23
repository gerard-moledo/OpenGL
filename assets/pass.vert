#version 330

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

out vec4 out_color;

uniform mat4 ortho_transform;

void main() {
    gl_Position = ortho_transform * vec4(in_position, 1.0f);
    out_color = vec4(in_color, 1.0f);
}