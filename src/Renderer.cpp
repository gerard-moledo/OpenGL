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

    glGenVertexArrays(1, &spec.vao);
    glBindVertexArray(spec.vao);

    glGenBuffers(1, &spec.buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, spec.buffer_object);

    GLint position_location = glGetAttribLocation(shader, "in_position");
    GLint color_location = glGetAttribLocation(shader, "in_color");

    int stride = (format.position_size + format.color_size) * sizeof(float);
    glVertexAttribPointer(  position_location, format.position_size,
                            GL_FLOAT, GL_FALSE,
                            stride, (void*) 0);
    glEnableVertexAttribArray(position_location);
    glVertexAttribPointer(  color_location, format.color_size, 
                            GL_FLOAT, GL_FALSE, 
                            stride, (void*) (format.position_size * sizeof(float)));
    glEnableVertexAttribArray(color_location);

    
    spec.format = format;
    spec.shader_program = shader;

    vao_spec_map.emplace(vao_handle, spec);
}

void Renderer::Update_VAO_Stream(std::string vao_handle) {
    VAO_Spec& spec = vao_spec_map.at(vao_handle);
    glBindVertexArray(spec.vao);
    glBindBuffer(GL_ARRAY_BUFFER, spec.buffer_object);

    glBufferData(GL_ARRAY_BUFFER, spec.stream.size() * sizeof(float), spec.stream.data(), GL_STATIC_DRAW);
}

void Renderer::Draw(std::string vao_handle) {
    VAO_Spec& spec = vao_spec_map.at(vao_handle);

    glUseProgram(spec.shader_program);
    glBindVertexArray(spec.vao);
    glBindBuffer(GL_ARRAY_BUFFER, spec.buffer_object);
    
    glDrawArrays(GL_TRIANGLES, 0, spec.stream.size() / spec.format.get_size());
}