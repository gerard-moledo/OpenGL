#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "glad/glad.h"

#include <string>
#include <vector>
#include <map>



struct Vertex_Format {
    int position_size;
    int color_size;

    int get_size() {
        return position_size + color_size;
    }
};

struct VAO_Spec {
    GLuint vao;
    GLuint buffer_object;
    GLuint shader_program;

    Vertex_Format format;
    std::vector<float> stream;
};

namespace Renderer {
    extern std::map<std::string, GLuint> shader_map;
    extern std::map<std::string, VAO_Spec> vao_spec_map;

    void Create_Shader(std::string filename);

    void Initialize_VAO(std::string vao_handle, GLuint shader, Vertex_Format format);

    void Update_VAO_Stream(std::string vao_handle);

    void Draw(std::string vao_handle);
};

#endif