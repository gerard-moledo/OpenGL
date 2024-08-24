#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <vector>

#include "Renderer.hpp"

class Sprite {
public:
    glm::vec2 position { 0.0f, 0.0f };
    glm::vec2 size { 5.0f, 5.0f };
    glm::vec3 color { 1.0f, 0.0f, 1.0f };

    std::vector<glm::vec3> mesh;
    glm::mat4 transform;

    VAO_Spec vao_spec;

    Sprite() {
        update_mesh();
    }

    void update_mesh();

    void update_buffer();
};

#endif