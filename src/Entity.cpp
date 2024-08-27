#include "Entity.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

Player::Player(glm::vec3 position) {
    transform.position = position;
    transform.scale = glm::vec3(100.0f);
    sprite.tint = { 1.0f, 0.0f, 0.0f };

    update_render();
}

void Player::update_render() {
    Vertex_Buffer& buffer = sprite.buffer;

    glm::mat4 matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, transform.position);
    matrix = glm::scale(matrix, transform.scale);

    sprite.update_buffer(matrix);
}