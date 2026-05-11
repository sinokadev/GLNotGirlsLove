#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

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
#define TITLE "여기에 TITLE을 써 넣으시오."

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

    float rotationSpeed = 300.0f;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) camera.rotate(-rotationSpeed * deltaTime, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) camera.rotate(rotationSpeed * deltaTime, 0.0f);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) camera.rotate(0.0f, rotationSpeed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) camera.rotate(0.0f, -rotationSpeed * deltaTime);

    std::cout << "Player Pos: " 
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
    
    glEnable(GL_MULTISAMPLE);  

    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);

    Shader shader("src/shaders/shader.vert", "src/shaders/shader.frag");

    std::vector<Vertex> vertices = {
        {{ 0.2f,  0.2f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        {{ 0.2f, -0.2f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.2f, -0.2f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.2f,  0.2f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
    };

    std::vector<unsigned int> indices = {
        0, 1, 3,   1, 2, 3,
    };

    Mesh mesh(vertices, indices);
    Object object(mesh, shader, glm::vec3(0), glm::vec3(0,0,0), glm::vec3(1));

    MovingCamera camera;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        // DeltaTime
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        process_input(window, camera, deltaTime);

        // Draw Plane
        shader.use();

        glm::mat4 view = camera.get_view_matrix();

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 100.0f);

        shader.set("view", view);
        shader.set("projection", projection);

        object.draw();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    shader.destroy();
    mesh.cleanup();

    glfwTerminate();
    return 0;
}