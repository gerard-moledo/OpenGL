#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>

#include "Renderer.hpp"
#include "Sprite.hpp"

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

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

int main() {
    // ==============================
    // GLFW Initialize
    // ==============================
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
    
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
    bool is_mode_lines = false;
    // ===============================
    Renderer::Create_Shader("pass");

    // ===============================
    // Batch Creation
    // ===============================
    constexpr int WIDTH = 86;
    constexpr int HEIGHT = 64;
    std::vector<Sprite> sprites;
    for (int i = 0; i < WIDTH * HEIGHT; ++i) {
        Sprite sprite;
        sprite.position = glm::vec2((i % WIDTH) * 10.0f - (WIDTH - 80) * 5, (i / WIDTH) * 10.0f - (HEIGHT - 60) * 5);
        sprite.color = glm::vec3(i % 3 / 3.0f, (i % 3 + 1) / 3.0f, (i % 3 + 2) / 3.0f);
        sprite.update_mesh();
        sprites.emplace_back(sprite);
    }

    Renderer::Initialize_VAO("batch", Renderer::shader_map.at("pass"), {3, 3});

    VAO_Spec& batch_spec = Renderer::vao_spec_map.at("batch");
    for (Sprite& sprite : sprites) {
        sprite.update_buffer();
        batch_spec.stream.insert(batch_spec.stream.end(), sprite.format.buffer.begin(), sprite.format.buffer.end());
    }
    //  ==============================

    // ===============================
    // Single Sprite 
    // ===============================
    Renderer::Initialize_VAO("single", Renderer::shader_map.at("pass"), Vertex_Format{ 3, 3 });

    VAO_Spec& single_spec = Renderer::vao_spec_map.at("single");
    single_spec.stream = std::vector<float> {
        100.0f, 100.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        300.0f, 100.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        300.0f, 300.0f, 0.0f, 1.0f, 1.0f, 0.0f,

        100.0f, 100.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        300.0f, 300.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        100.0f, 300.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    };

    Renderer::Update_VAO_Stream("single");
    // ===============================

    // Setup screen-space transform
    glUseProgram(single_spec.shader_program);

    unsigned int ORTHO_TRANSFORM_LOCATION = glGetUniformLocation(Renderer::shader_map.at("pass"), "ortho_transform");

    glm::mat4 ortho_transform = glm::mat4(1.0f);
    ortho_transform = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.0f, -100.0f) * ortho_transform;
    glUniformMatrix4fv(ORTHO_TRANSFORM_LOCATION, 1, GL_FALSE, glm::value_ptr(ortho_transform));

    glUseProgram(0);
    // =============================

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

            constexpr int SPEED = 2000;

            auto& stream = Renderer::vao_spec_map.at("batch").stream;
            auto stream_it = stream.begin();
            for (Sprite& sprite : sprites) {
                glm::vec2 position = sprite.position;
                sprite.position += glm::vec2(1.0f, 1.0f) * 40.0f * (((int) (current_t * SPEED) % SPEED) / (float) SPEED - 0.5f);
                sprite.size = glm::vec2(1.0f) * 20.0f * -glm::abs(((int) (current_t * SPEED) % SPEED) / (float) SPEED - 0.5f);
                sprite.update_buffer();
                stream_it = std::copy(sprite.format.buffer.begin(), sprite.format.buffer.end(), stream_it);
                sprite.position = position;
            }
        }

        // ===============================
        // RENDERING
        // ===============================
        glClear(GL_COLOR_BUFFER_BIT);

        Renderer::Update_VAO_Stream("batch");
        Renderer::Draw("batch");
        
        Renderer::Draw("single");

        printf("%d\n", (int) (1 / dt));
        glfwSwapBuffers(window);
        // ===============================
    }

    glfwTerminate();

    return 0;
}