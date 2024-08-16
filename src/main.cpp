#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

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

GLuint CreateShader(std::string filename) {
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
    glGetShaderiv(program, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);

    bool gl_loaded = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    if (!gl_loaded) {
        printf("Failed to initialize GLAD\n");
        glfwTerminate();
        return -1;
    }
    
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    GLuint pass_shader = CreateShader("pass");
    glUseProgram(pass_shader);

//  ==========VERTEX DATA CREATION==========
    constexpr int POSITION_LOCATION = 0;
    constexpr int VERTEX_SIZE = 4;

    std::vector<float> triangle{
        100.0f, 100.0f, 0.0f, 1.0f,
        400.0f, 500.0f, 0.0f, 1.0f,
        700.0f, 100.0f, 0.0f, 1.0f,
    };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint array_buffer;
    glGenBuffers(1, &array_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
    glBufferData(GL_ARRAY_BUFFER, triangle.size() * sizeof(float), triangle.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(POSITION_LOCATION, triangle.size() / VERTEX_SIZE, GL_FLOAT, GL_FALSE, VERTEX_SIZE * sizeof(float), nullptr);
    glEnableVertexAttribArray(POSITION_LOCATION);

    glBindVertexArray(0);
//  ========================================

    unsigned int PIXEL_TRANSFORM_LOCATION = glGetUniformLocation(pass_shader, "pixel_transform");
    glm::mat4 pixel_transform = glm::mat4(1.0f);
    pixel_transform = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.0f, -100.0f) * pixel_transform;
    glUniformMatrix4fv(PIXEL_TRANSFORM_LOCATION, 1, false, glm::value_ptr(pixel_transform));

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);

        glDrawArrays(GL_TRIANGLES, 0, triangle.size() / VERTEX_SIZE);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}