#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

struct Buffer_Format {
    std::vector<float> buffer;

    void fill_buffer(std::vector<Vertex>& vertices);
};

class Sprite {
public:
    glm::vec2 position { 0.0f, 0.0f };
    glm::vec2 size { 5.0f, 5.0f };
    glm::vec3 color { 1.0f, 0.0f, 1.0f };

    std::vector<float> quad_2_positions {
        0.0f, 0.0f,   1.0f, 0.0f,   1.0f, 1.0f,
        0.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f,
    };

    std::vector<Vertex> mesh;
    std::vector<Vertex> transformed_mesh;
    glm::mat4 transform;
    Buffer_Format format;

    Sprite() {
        update_mesh();
    }

    void update_mesh();

    void update_buffer();
};