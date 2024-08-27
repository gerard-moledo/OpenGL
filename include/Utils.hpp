#ifndef UTILS_HPP
#define UTILS_HPP

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <vector>

struct Vertex_Buffer {
    GLuint buffer_object;
    GLenum primitive;
    
    int position_size = 0;
    std::vector<float> position;
    int color_size = 0;
    std::vector<float> color;
    int texcoord_size = 0;
    std::vector<float> texcoord;

    std::vector<float> stream;

    int get_size() {
        return position_size + color_size + texcoord_size;
    }
};

struct Sprite {
    std::vector<glm::vec3> mesh = {
        { -0.5f, -0.5f, 0.0f },
        {  0.5f, -0.5f, 0.0f },
        {  0.5f,  0.5f, 0.0f },

        { -0.5f, -0.5f, 0.0f },
        {  0.5f,  0.5f, 0.0f },
        { -0.5f,  0.5f, 0.0f },
    };

    glm::vec3 tint = { 1.0f, 1.0f, 1.0f };
    //std::vector<glm::vec2> texcoord;

    Vertex_Buffer buffer;
    const int BUFFER_POSITION_SIZE = 3;
    const int BUFFER_COLOR_SIZE = 3;
    const int BUFFER_TEXCOORD_SIZE = 0;

    Sprite() {
        buffer.primitive = GL_TRIANGLES;
        
        buffer.position_size = BUFFER_POSITION_SIZE;
        buffer.position.resize(mesh.size() * BUFFER_POSITION_SIZE);

        buffer.color_size = BUFFER_COLOR_SIZE;
        buffer.color.resize(mesh.size() * BUFFER_COLOR_SIZE);
        
        buffer.texcoord_size = BUFFER_TEXCOORD_SIZE;
    }

    void update_buffer(glm::mat4 transform_matrix) {
        buffer.stream.resize(buffer.position.size() + buffer.color.size() + buffer.texcoord.size());

        for (int v = 0; v < buffer.stream.size() / buffer.get_size(); ++v) {
            glm::vec3 transformed_position = transform_matrix * glm::vec4(mesh[v], 1.0f);

            buffer.position[BUFFER_POSITION_SIZE * v + 0] = transformed_position.x;
            buffer.position[BUFFER_POSITION_SIZE * v + 1] = transformed_position.y;
            buffer.position[BUFFER_POSITION_SIZE * v + 2] = transformed_position.z;

            buffer.color[BUFFER_COLOR_SIZE * v + 0] = tint.x;
            buffer.color[BUFFER_COLOR_SIZE * v + 1] = tint.y;
            buffer.color[BUFFER_COLOR_SIZE * v + 2] = tint.z;


            int position_offset = buffer.get_size() * v;
            for (int p = 0; p < buffer.position_size; ++p) {
                buffer.stream[position_offset + p] = buffer.position[v * BUFFER_POSITION_SIZE + p];
            }

            int color_offset = position_offset + BUFFER_COLOR_SIZE;
            for (int c = 0; c < buffer.color_size; ++c) {
                buffer.stream[color_offset + c] = buffer.color[v * BUFFER_COLOR_SIZE + c];
            }

            // int texcoord_offset = color_offset + 0;
            // for (int t = 0; t < buffer.texcoord.size(); ++t) {
            //     buffer.stream[texcoord_offset + t] = buffer.texcoord[v * buffer.texcoord_size + t];
            // }
        }
    }
};

struct Transform {
    glm::vec3 position;
    glm::vec3 scale;
};

#endif