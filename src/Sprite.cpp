#include "Sprite.hpp"

void Buffer_Format::fill_buffer(std::vector<Vertex>& vertices) {
    buffer.resize(vertices.size() * 6);
    for (int v = 0; v < vertices.size(); ++v) {
        buffer[v * 6 + 0] = vertices[v].position.x;
        buffer[v * 6 + 1] = vertices[v].position.y;
        buffer[v * 6 + 2] = vertices[v].position.z;
        buffer[v * 6 + 3] = vertices[v].color.r;
        buffer[v * 6 + 4] = vertices[v].color.g;
        buffer[v * 6 + 5] = vertices[v].color.b;
    }
}

void Sprite::update_mesh() {
    mesh.resize(quad_2_positions.size() / 2);
    for (int p = 0; p < quad_2_positions.size() / 2; ++p) {
        Vertex vertex;
        vertex.position = glm::vec3(quad_2_positions[p * 2], quad_2_positions[p * 2 + 1], 0.0f);
        vertex.color = color;
        mesh[p] = vertex;
    }
}

void Sprite::update_buffer() {
    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(position - size / 2.0f, 0.0f));
    transform = glm::scale(transform, glm::vec3(size, 1.0f));
    
    transformed_mesh.resize(mesh.size());
    for (int v = 0; v < mesh.size(); ++v) {
        Vertex vertex;
        vertex.position = transform * glm::vec4(mesh[v].position, 1.0f);
        vertex.color = mesh[v].color;
        transformed_mesh[v] = vertex;
    }
    format.fill_buffer(transformed_mesh);
}