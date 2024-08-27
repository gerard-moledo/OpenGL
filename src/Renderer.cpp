#include "Renderer.hpp"

#include <map>
#include <iostream>
#include <fstream>

namespace Renderer {
    std::map<std::string, GLuint> shader_map;
    std::map<std::string, GLuint> vao_map;
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

void Renderer::Initialize_VAO(std::string vao_handle, GLuint shader, Vertex_Buffer& buffer_format) {
    GLuint vao;
    GLuint& buffer_object = buffer_format.buffer_object;

    // Generate vertex objects
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &buffer_object);

    // Use VAO to determine vertex attributes
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_object);

    // Get the locations of attributes used in this shader
    GLint position_location = glGetAttribLocation(shader, "in_position");
    GLint color_location = glGetAttribLocation(shader, "in_color");
    GLint texcoord_location = glGetAttribLocation(shader, "in_texcoord");

    size_t offset = 0;
    if (position_location >= 0) {
        glVertexAttribPointer(  position_location, buffer_format.position_size,
                                GL_FLOAT, GL_FALSE,
                                buffer_format.get_size() * sizeof(float),
                                (void*) offset);
        glEnableVertexAttribArray(position_location);
        offset += buffer_format.position_size;
    }
    if (color_location >= 0) {
        glVertexAttribPointer(  color_location, buffer_format.color_size,
                                GL_FLOAT, GL_FALSE,
                                 buffer_format.get_size() * sizeof(float),
                                 (void*) offset);
        glEnableVertexAttribArray(color_location);
        offset += buffer_format.color_size;
    }
    if  (texcoord_location >= 0) {
        glVertexAttribPointer(  texcoord_location, buffer_format.texcoord_size,
                                GL_FLOAT, GL_FALSE,
                                buffer_format.get_size() * sizeof(float),
                                (void*) offset);
        glEnableVertexAttribArray(texcoord_location);
        offset += buffer_format.texcoord_size;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    vao_map.emplace(vao_handle, vao);

    return;
}

void Renderer::Update_VAO_Buffer(std::string vao_handle, Vertex_Buffer& buffer) {
    GLuint& vao = vao_map[vao_handle];
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, buffer.buffer_object);
    glBufferData(GL_ARRAY_BUFFER, buffer.stream.size() * sizeof(float), buffer.stream.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::Draw(std::string vao_handle, Vertex_Buffer& buffer) {
    GLuint& vao = vao_map[vao_handle];

    glBindVertexArray(vao);

    int primitive_count = 0;
    if (buffer.primitive == GL_TRIANGLES)
        primitive_count = buffer.stream.size() / buffer.get_size();

    glDrawArrays(buffer.primitive, 0, primitive_count);

    glBindVertexArray(0);
}