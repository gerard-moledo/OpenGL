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

struct Input {
    bool space_pressed;
} user_input;

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        user_input.space_pressed = true;
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
        user_input.space_pressed = false;
    }
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
    
    glfwSetKeyCallback(window, keyboard_callback);

    glfwMakeContextCurrent(window);

    bool gl_loaded = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    if (!gl_loaded) {
        printf("Failed to initialize GLAD\n");
        glfwTerminate();
        return -1;
    }
    
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
    bool is_mode_lines = false;

    GLuint pass_shader = CreateShader("pass");
    glUseProgram(pass_shader);
    constexpr int POSITION_LOCATION = 0;
    constexpr int COLOR_LOCATION = 1;

//  ==========VERTEX DATA CREATION - QUAD==========
    constexpr int VERTEX_SIZE = 5;
    
    std::vector<float> quad {
    //  Pos          Color 
        0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
        1.0f, 1.0f,  0.0f, 1.0f, 0.0f,

        0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
        0.0f, 1.0f,  1.0f, 1.0f, 0.0f,
    };

    struct Sprite {
        glm::vec2 position { 100.0f, 100.0f };
        glm::vec2 size { 200.0f, 300.0f };
        
        GLuint vao;
        GLuint buffer;
    } sprite;

    glGenVertexArrays(1, &sprite.vao);
    glBindVertexArray(sprite.vao);

    glGenBuffers(1, &sprite.buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sprite.buffer);
    glBufferData(GL_ARRAY_BUFFER, quad.size() * sizeof(float), quad.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(POSITION_LOCATION, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(COLOR_LOCATION, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE * sizeof(float), (void*) (2 * sizeof(float)) );
    glEnableVertexAttribArray(COLOR_LOCATION);

    glBindVertexArray(0);
//  ========================================

    unsigned int WORLD_TRANSFORM_LOCATION = glGetUniformLocation(pass_shader, "world_transform");
    unsigned int ORTHO_TRANSFORM_LOCATION = glGetUniformLocation(pass_shader, "ortho_transform");

    glm::mat4 ortho_transform = glm::mat4(1.0f);
    ortho_transform = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.0f, -100.0f) * ortho_transform;
    glUniformMatrix4fv(ORTHO_TRANSFORM_LOCATION, 1, GL_FALSE, glm::value_ptr(ortho_transform));

    float prev_t = (float) glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        user_input.space_pressed = false;
        glfwPollEvents();

        float current_t = (float) glfwGetTime();
        float dt = current_t - prev_t;
        prev_t = current_t;

        double d_mouse_x, d_mouse_y;
        glfwGetCursorPos(window, &d_mouse_x, &d_mouse_y);

        sprite.position.x = (float) d_mouse_x - sprite.size.x / 2.0f;
        sprite.position.y = (float) (WINDOW_HEIGHT - d_mouse_y) - sprite.size.y / 2.0f;

        if (user_input.space_pressed) {
            is_mode_lines = !is_mode_lines;
            if (is_mode_lines)  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        glClear(GL_COLOR_BUFFER_BIT);
        
        // DRAW QUAD
        glm::mat4 world_transform = glm::mat4(1.0f);
        world_transform = glm::translate(world_transform, glm::vec3(sprite.position, 0.0f));
        world_transform = glm::scale(world_transform, glm::vec3(sprite.size, 1.0f));
        glUniformMatrix4fv(WORLD_TRANSFORM_LOCATION, 1, GL_FALSE, glm::value_ptr(world_transform));

        glBindVertexArray(sprite.vao);

        glDrawArrays(GL_TRIANGLES, 0, quad.size() / VERTEX_SIZE);
        // END QUAD

        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}