#include "Renderer.hpp"

#include <iostream>
#include <fstream>

namespace Renderer {
    std::map<std::string, GLuint> shader_map;
    std::map<std::string, VAO_Spec> vao_spec_map;
}

void ReadShaderFromFile(std::string& source, std::string file) {
    source.clear();

    std::ifstream filestream {};
    filestream.open(file);

    std::string line {};
    while (std::getline(filestream, line)) {
        source.append(line + "\n");
    }

    filestream.close();
}

void Renderer::Create_Shader(std::string filename) {
    std::string vertex_source {};
    std::string fragment_source {};

    ReadShaderFromFile(vertex_source, "assets/" + filename + ".vert");
    ReadShaderFromFile(fragment_source, "assets/" + filename + ".frag");

    const GLchar* vertex_string = vertex_source.c_str();
    const GLchar* fragment_string = fragment_source.c_str();

    int success;
    char infoLog[512];

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_string, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_string, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    shader_map.emplace(filename, program);
}

void Renderer::Initialize_VAO(std::string vao_handle, GLuint shader, Vertex_Format format) {
    VAO_Spec spec;

    // Generate vertex objects
    glGenVertexArrays(1, &spec.vao);
    glGenBuffers(1, &spec.buffer_object);

    // Use VAO to determine vertex attributes
    glBindVertexArray(spec.vao);

    // Get the locations of attributes used in this shader
    // TODO: Figure out way to avoid hardcoding shader locations, or more elegantly handle
    //       different possible shaders.
    GLint position_location = glGetAttribLocation(shader, "in_position");
    GLint color_location = glGetAttribLocation(shader, "in_color");

    glBindBuffer(GL_ARRAY_BUFFER, spec.buffer_object);
    glVertexAttribPointer(  position_location, format.position_size,
                            GL_FLOAT, GL_FALSE,
                            format.get_size() * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(position_location);
    glVertexAttribPointer(  color_location, format.color_size,
                            GL_FLOAT, GL_FALSE,
                            format.get_size() * sizeof(float), (void*)(format.position_size * sizeof(float)));
    glEnableVertexAttribArray(color_location);

    // Assumes 4x4 matrix instancing
    // TODO (cont.): Handle different shaders and their attributes more elegantly.
    if (shader == shader_map["instanced"]) {
        constexpr int MAT4_SIZE = 4;
        GLint offset_location = glGetAttribLocation(shader, "in_instance_transform");
        glGenBuffers(1, &spec.instanced_buffer_object);
        glBindBuffer(GL_ARRAY_BUFFER, spec.instanced_buffer_object);
        for (int i = 0; i < MAT4_SIZE; ++i) {
            glVertexAttribPointer(  offset_location + i, MAT4_SIZE,
                                    GL_FLOAT, GL_FALSE,
                                    MAT4_SIZE * MAT4_SIZE * sizeof(float), (void*)(i * MAT4_SIZE * sizeof(float)));
            glEnableVertexAttribArray(offset_location + i);
            glVertexAttribDivisor(offset_location + i, 1);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    spec.format = format;
    spec.shader_program = shader;

    vao_spec_map.emplace(vao_handle, spec);
}

void Renderer::Update_VAO_Stream(std::string vao_handle) {
    VAO_Spec& spec = vao_spec_map.at(vao_handle);
    glBindVertexArray(spec.vao);

    glBindBuffer(GL_ARRAY_BUFFER, spec.buffer_object);
    glBufferData(GL_ARRAY_BUFFER, spec.stream.size() * sizeof(float), spec.stream.data(), GL_STATIC_DRAW);

    if (spec.instanced_buffer_object != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, spec.instanced_buffer_object);
        glBufferData(GL_ARRAY_BUFFER, spec.instanced_stream.size() * sizeof(float), spec.instanced_stream.data(), GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::Draw(std::string vao_handle) {
    VAO_Spec& spec = vao_spec_map.at(vao_handle);

    glBindVertexArray(spec.vao);

    if (spec.instance_count == 0) {
        glDrawArrays(GL_TRIANGLES, 0, spec.stream.size() / spec.format.get_size());
    } else {
        glDrawArraysInstanced(GL_TRIANGLES, 0, spec.stream.size() / spec.format.get_size(), spec.instance_count);
    }

    glBindVertexArray(0);
}