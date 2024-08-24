#include "Sprite.hpp"

#include "Utils.hpp"

// TODO: Counts manually how many vertices are part of buffer definition. Should be changed.
constexpr int VERTEX_ATTRIBUTE_COUNT = 2;

void Sprite::update_mesh() {
    Model<glm::vec3> model = Models::quad_3d;

    mesh.resize(model.vertex_count * VERTEX_ATTRIBUTE_COUNT);
    for (int v = 0; v < model.vertex_count; ++v) {
        mesh[v * VERTEX_ATTRIBUTE_COUNT] = model.mesh[v];

        mesh[v * VERTEX_ATTRIBUTE_COUNT + 1] = color;
    }
}

void Sprite::update_buffer() {
    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(position - size / 2.0f, 0.0f));
    transform = glm::scale(transform, glm::vec3(size, 1.0f));
    
    
    vao_spec.stream.resize(mesh.size() * vao_spec.format.get_size());
    for (int v = 0; v < mesh.size() / 2; ++v) {
        glm::vec4 transformed_mesh = transform * glm::vec4(mesh[v * VERTEX_ATTRIBUTE_COUNT], 1.0f);
        vao_spec.stream[v * vao_spec.format.get_size() + 0] = transformed_mesh.x;
        vao_spec.stream[v * vao_spec.format.get_size() + 1] = transformed_mesh.y;
        vao_spec.stream[v * vao_spec.format.get_size() + 2] = transformed_mesh.z;

        vao_spec.stream[v * vao_spec.format.get_size() + 3] = mesh[v * VERTEX_ATTRIBUTE_COUNT + 1].x;
        vao_spec.stream[v * vao_spec.format.get_size() + 4] = mesh[v * VERTEX_ATTRIBUTE_COUNT + 1].y;
        vao_spec.stream[v * vao_spec.format.get_size() + 5] = mesh[v * VERTEX_ATTRIBUTE_COUNT + 1].z;
    }
}