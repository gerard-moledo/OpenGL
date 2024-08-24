#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "glad/glad.h"

#include <string>
#include <vector>
#include <map>

struct Vertex_Format {
    int position_size = 0;
    int color_size = 0;

    int get_size() {
        return position_size + color_size;
    }
};

struct VAO_Spec {
    GLuint vao = 0;
    GLuint buffer_object = 0;
    GLuint instanced_buffer_object = 0;
    GLuint shader_program = 0;

    Vertex_Format format;
    std::vector<float> stream;
    std::vector<float> instanced_stream;

    int instance_count = 0;
};

namespace Renderer {
    extern std::map<std::string, GLuint> shader_map;
    extern std::map<std::string, VAO_Spec> vao_spec_map;

    void Create_Shader(std::string filename);

    VAO_Spec Initialize_VAO(GLuint shader, Vertex_Format format);

    void Update_VAO_Stream(VAO_Spec& spec);

    void Draw(VAO_Spec& spec);
};

#endif