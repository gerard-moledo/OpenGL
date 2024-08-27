#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>

#include "Renderer.hpp"
#include "Entity.hpp"

// ================================
// Input Handling
// ================================
struct Input {
    bool space_pressed;
    bool up_pressed;
    bool down_pressed;
} user_input;

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        user_input.space_pressed = true;
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
        user_input.space_pressed = false;
    }
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        user_input.up_pressed = true;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) {
        user_input.down_pressed = true;
    }
}
// =================================

// GL Debugging
void debug_output_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    printf("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
        type, severity, message );
}
// =================================

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

int main() {
    // ==============================
    // GLFW Initialize
    // ==============================
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
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
    // ===============================

    // ===============================
    // GL Initialization
    // ===============================
    bool gl_loaded = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    if (!gl_loaded) {
        printf("Failed to initialize GLAD\n");
        glfwTerminate();
        return -1;
    }
    
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(debug_output_callback, (void*) 0);
    
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
    bool is_mode_lines = false;
    // ===============================

    // Shader Creation
    Renderer::Create_Shader("color");
    //Renderer::Create_Shader("sprite");

    // ===============================
    // Setup screen-space transform
    // ===============================
    glUseProgram(Renderer::shader_map["color"]);

    unsigned int ORTHO_TRANSFORM_LOCATION = glGetUniformLocation(Renderer::shader_map["color"], "ortho_transform");
    glm::mat4 ortho_transform = glm::mat4(1.0f);
    ortho_transform = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.0f, -100.0f) * ortho_transform;
    glUniformMatrix4fv(ORTHO_TRANSFORM_LOCATION, 1, GL_FALSE, glm::value_ptr(ortho_transform));

    glUseProgram(0);
    // =============================

    // Ideal entity creation code
    Player player = Player(glm::vec3(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, 0.0f));
    // Text dialogue = Text(text);
    // Solid wall = Solid(shape);

    Renderer::Initialize_VAO("player", Renderer::shader_map["color"], player.sprite.buffer);
    Renderer::Update_VAO_Buffer("player", player.sprite.buffer);

    // =============================
    // GAME LOOP
    // =============================
    float prev_t = (float) glfwGetTime();
    float frame_t = 0.0f;
    float frame_rate = 1 / 60.0f;
    int lag = 1;
    while (!glfwWindowShouldClose(window)) {
        // Reset single frame inputs
        user_input.space_pressed = false;
        user_input.down_pressed = false;
        user_input.up_pressed = false;

        glfwPollEvents();

        // Test code for modulating lag
        if (user_input.up_pressed)      printf("\nLAG: %d\n\n", ++lag);
        if (user_input.down_pressed)    printf("\nLAG: %d\n\n", --lag);

        // Timing
        float current_t = (float) glfwGetTime();
        float dt = current_t - prev_t;
        //printf("%.3f\n", (float) dt);
        prev_t = current_t;
        if (dt > frame_rate * lag && lag > 0)
            dt = frame_rate * lag;
        frame_t += dt;

        // Toggle wireframes
        if (user_input.space_pressed) {
            is_mode_lines = !is_mode_lines;
            if (is_mode_lines)  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // Update Loop
        while (frame_t > frame_rate) {
            frame_t -= frame_rate;

        }

        // ===============================
        // RENDERING
        // ===============================
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(Renderer::shader_map["color"]);

        Renderer::Draw("player", player.sprite.buffer);

        glUseProgram(0);


        glfwSwapBuffers(window);
        // ===============================
    }

    glfwTerminate();

    return 0;
}