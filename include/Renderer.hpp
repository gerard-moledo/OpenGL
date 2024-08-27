#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "glad/glad.h"

#include <string>
#include <vector>
#include <map>
#include "Utils.hpp"

namespace Renderer {
    extern std::map<std::string, GLuint> shader_map;
    extern std::map<std::string, GLuint> vao_map;

    void Create_Shader(std::string filename);

    void Initialize_VAO(std::string vao_handle, GLuint shader, Vertex_Buffer& buffer_format);

    void Update_VAO_Buffer(std::string vao_handle, Vertex_Buffer& buffer);

    void Draw(std::string vao_handle, Vertex_Buffer& buffer);
};

#endif