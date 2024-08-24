#ifndef UTILS_HPP
#define UTILS_HPP

#include "glm/glm.hpp"

#include <vector>

template <typename T> 
struct Model {
    int vertex_count;
    int vertex_size;

    std::vector<T> mesh;
};

namespace Models {
    Model<glm::vec3> quad_3d {
        6, 3,
        {
            { 0.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f },
            { 1.0f, 1.0f, 0.0f },

            { 0.0f, 0.0f, 0.0f },
            { 1.0f, 1.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
        },
    };
}

#endif