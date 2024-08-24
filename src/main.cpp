#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>

#include "Renderer.hpp"
#include "Sprite.hpp"

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
    Renderer::Create_Shader("pass");
    Renderer::Create_Shader("instanced");

    // ===============================
    // Batch Creation
    // ===============================
    constexpr int WIDTH = 104;
    constexpr int HEIGHT = 104;
    std::vector<Sprite> sprites;
    for (int i = 0; i < WIDTH * HEIGHT; ++i) {
        Sprite sprite;
        sprite.vao_spec = Renderer::Initialize_VAO(Renderer::shader_map["pass"], Vertex_Format{3, 3});
        sprite.position = glm::vec2((i % WIDTH) * 10.0f - 20, (i / WIDTH) * 10.0f - 20);
        sprite.color = glm::vec3(i % 3 / 3.0f, (i % 3 + 1) / 3.0f, (i % 3 + 2) / 3.0f);
        sprite.update_mesh();
        
        sprites.emplace_back(sprite);
    }

    VAO_Spec batch_vao_spec = Renderer::Initialize_VAO(Renderer::shader_map["pass"], Vertex_Format{3, 3});
    for (Sprite& sprite : sprites) {
        sprite.update_buffer();
        batch_vao_spec.stream.insert(batch_vao_spec.stream.end(), sprite.format.buffer.begin(), sprite.format.buffer.end());
        sprite.vao_spec.stream = sprite.format.buffer;
    }
    
    Renderer::vao_spec_map.emplace("batch", batch_vao_spec);
    // ===============================

    // ===============================
    // Individual Sprites
    // ===============================
    VAO_Spec single_vao_spec = Renderer::Initialize_VAO(Renderer::shader_map["pass"], Vertex_Format{ 3, 3 });
    single_vao_spec.stream = std::vector<float> {
        100.0f, 100.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        300.0f, 100.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        300.0f, 300.0f, 0.0f, 1.0f, 1.0f, 0.0f,

        100.0f, 100.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        300.0f, 300.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        100.0f, 300.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    };

    Renderer::vao_spec_map.emplace("single", single_vao_spec);

    Renderer::Update_VAO_Stream(single_vao_spec);
    // ===============================

    // ===============================
    // Instanced Sprites 
    // =============================== 
    VAO_Spec instanced_vao_spec = Renderer::Initialize_VAO(Renderer::shader_map["instanced"], Vertex_Format{ 3, 3 });

    instanced_vao_spec.stream = std::vector<float> {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
    };
    
    constexpr int INSTANCE_WIDTH = 104;
    constexpr int INSTANCE_HEIGHT = 104;
    instanced_vao_spec.instance_count = INSTANCE_WIDTH * INSTANCE_HEIGHT;
    for (int i = 0; i < instanced_vao_spec.instance_count; ++i) {
        glm::mat4 world_transform = glm::mat4(1.0f);
        glm::vec3 offset = glm::vec3(   800.0f / INSTANCE_WIDTH * (i % INSTANCE_WIDTH + 0.5f), 
                                        600.0f / INSTANCE_HEIGHT * (i / INSTANCE_WIDTH + 0.5f),
                                        0.0f);
        world_transform = glm::translate(world_transform, offset);
        float* transform_data = glm::value_ptr(world_transform);
        instanced_vao_spec.instanced_stream.insert( instanced_vao_spec.instanced_stream.begin() + 16 * i,
                                                    transform_data + 0, transform_data + 16);
    }

    Renderer::vao_spec_map.emplace("single", instanced_vao_spec);
    Renderer::Update_VAO_Stream(instanced_vao_spec);
    // =============================== 

    // ===============================
    // Setup screen-space transform
    // ===============================
    glUseProgram(single_vao_spec.shader_program);

    unsigned int ORTHO_TRANSFORM_LOCATION = glGetUniformLocation(Renderer::shader_map["pass"], "ortho_transform");
    glm::mat4 ortho_transform = glm::mat4(1.0f);
    ortho_transform = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.0f, -100.0f) * ortho_transform;
    glUniformMatrix4fv(ORTHO_TRANSFORM_LOCATION, 1, GL_FALSE, glm::value_ptr(ortho_transform));

    
    glUseProgram(instanced_vao_spec.shader_program);

    unsigned int ORTHO_INSTANCE_TRANSFORM_LOCATION = glGetUniformLocation(Renderer::shader_map["instanced"], "ortho_transform");
    ortho_transform = glm::mat4(1.0f);
    ortho_transform = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.0f, -100.0f) * ortho_transform;
    glUniformMatrix4fv(ORTHO_INSTANCE_TRANSFORM_LOCATION, 1, GL_FALSE, glm::value_ptr(ortho_transform));

    unsigned int WORLD_INSTANCE_TRANSFORM_LOCATION = glGetUniformLocation(Renderer::shader_map["instanced"], "world_transform");
    glm::mat4 world_instance_transform;
    
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
        printf("%.3f\n", (float) dt);
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

            auto& stream = batch_vao_spec.stream;
            auto stream_it = stream.begin();
            for (Sprite& sprite : sprites) {
                glm::vec2 position = sprite.position;
                sprite.position += glm::vec2(1.0f, 1.0f) * 40.0f * (((int) (current_t * SPEED) % SPEED) / (float) SPEED - 0.5f);
                sprite.size = glm::vec2(1.0f) * 20.0f * -glm::abs(((int) (current_t * SPEED) % SPEED) / (float) SPEED - 0.5f);
                sprite.update_buffer();
                stream_it = std::copy(sprite.format.buffer.begin(), sprite.format.buffer.end(), stream_it);
                sprite.position = position;
                // Part of individual sprite testing
                // sprite.vao_spec.stream = sprite.format.buffer;
            }

            world_instance_transform = glm::mat4(1.0f);
            world_instance_transform = glm::rotate(world_instance_transform, 0.75f * glm::pi<float>() * fmodf(current_t, 1000.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            glm::vec2 size = glm::vec2(1.0f) * 10.0f * -glm::abs(((int) (current_t * SPEED) % SPEED) / (float) SPEED - 0.5f);
            world_instance_transform = glm::scale(world_instance_transform, glm::vec3(size, 1.0f));
        }

        // ===============================
        // RENDERING
        // ===============================
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(Renderer::shader_map["pass"]);
        
        // Rendering sprites individually
        /* Done for testing, will be removed 
        for (Sprite& sprite : sprites) {
            Renderer::Update_VAO_Stream(sprite.vao_spec);
            Renderer::Draw(sprite.vao_spec);
        }
        */

        Renderer::Update_VAO_Stream(batch_vao_spec);
        Renderer::Draw(batch_vao_spec);
        
        Renderer::Draw(single_vao_spec);


        glUseProgram(Renderer::shader_map["instanced"]);
        
        glUniformMatrix4fv(WORLD_INSTANCE_TRANSFORM_LOCATION, 1, GL_FALSE, glm::value_ptr(world_instance_transform));
        Renderer::Draw(instanced_vao_spec);
        
        glUseProgram(0);

        glfwSwapBuffers(window);
        // ===============================
    }

    glfwTerminate();

    return 0;
}