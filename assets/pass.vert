#version 330

layout(location = 0) in vec4 in_position;

uniform mat4 pixel_transform;

void main() {
    gl_Position = pixel_transform * in_position;
}