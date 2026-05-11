#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "mesh.h"
#include "object.h"
#include "camera.hpp"

#define WIDTH 640
#define HEIGHT 480
#define TITLE "CheeseBlock"

int window_width = WIDTH;
int window_height = HEIGHT;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    window_width = width;
    window_height = height;
}

void process_input(GLFWwindow *window, MovingCamera &camera, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.move(camera.front, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.move(-camera.front, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.move(-camera.right, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.move(camera.right, deltaTime);

    float rotation_speed = 300.0f;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) camera.rotate(-rotation_speed * deltaTime, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) camera.rotate(rotation_speed * deltaTime, 0.0f);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) camera.rotate(0.0f, rotation_speed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) camera.rotate(0.0f, -rotation_speed * deltaTime);

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) std::cout << "Player Pos: "
        << std::fixed << std::setprecision(3)
        << camera.position.x << ", " 
        << camera.position.y << ", " 
        << camera.position.z << std::endl;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);

    Shader shader("src/shaders/shader.vert", "src/shaders/shader.frag");

    std::vector<Vertex> vertices = {
        // Front Face
        {{ 0.2f,  0.2f,  0.2f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        {{ 0.2f, -0.2f,  0.2f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.2f, -0.2f,  0.2f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.2f,  0.2f,  0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},

        // Back Face
        {{ 0.2f,  0.2f, -0.2f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
        {{ 0.2f, -0.2f, -0.2f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{-0.2f, -0.2f, -0.2f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{-0.2f,  0.2f, -0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},

        // Top Face
        {{ 0.2f,  0.2f, -0.2f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.2f,  0.2f,  0.2f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.2f,  0.2f,  0.2f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.2f,  0.2f, -0.2f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},

        // Bottom Face
        {{ 0.2f, -0.2f, -0.2f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
        {{ 0.2f, -0.2f,  0.2f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
        {{-0.2f, -0.2f,  0.2f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
        {{-0.2f, -0.2f, -0.2f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},

        // Right Face
        {{ 0.2f,  0.2f, -0.2f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.2f, -0.2f, -0.2f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.2f, -0.2f,  0.2f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.2f,  0.2f,  0.2f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},

        // Left Face
        {{-0.2f,  0.2f, -0.2f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.2f, -0.2f, -0.2f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.2f, -0.2f,  0.2f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.2f,  0.2f,  0.2f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
    };

    std::vector<unsigned int> indices = {
        0,  1,  3,   1,  2,  3,
        4,  5,  7,   5,  6,  7,
        8,  9,  11,  9,  10, 11,
        12, 13, 15,  13, 14, 15,
        16, 17, 19,  17, 18, 19,
        20, 21, 23,  21, 22, 23
    };

    Mesh mesh(vertices, indices);
    Object object(mesh, shader, glm::vec3(0), glm::vec3(0,0,0), glm::vec3(1));

    MovingCamera camera;

    float delta_time = 0.0f;
    float last_frame = 0.0f;
    
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        // DeltaTime
        float currentFrame = static_cast<float>(glfwGetTime());
        delta_time = currentFrame - last_frame;
        last_frame = currentFrame;

        int now_time = glfwGetTime();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        process_input(window, camera, delta_time);

        // Draw Plane
        shader.use();

        glm::mat4 view = camera.get_view_matrix();

        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)window_width / (float)window_height, 0.1f, 100.0f);

        shader.set("view", view);
        shader.set("projection", projection);

        object.rotation.y += 50.0f * delta_time;
        if (object.rotation.y > 360.0f) object.rotation.y -= 360.0f;

        object.draw();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    shader.destroy();
    mesh.cleanup();

    glfwTerminate();
    return 0;
}