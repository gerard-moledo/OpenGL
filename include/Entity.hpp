#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <vector>

#include "Renderer.hpp"
#include "Utils.hpp"

class Player {
public:
    Transform transform;
    Sprite sprite;

    Player(glm::vec3 position);

    void update_render();
};

#endif