#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "mesh.h"
#include "object.h"

#define WIDTH 640
#define HEIGHT 480
#define TITLE "여기에 TITLE을 써 넣으시오."

class Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;

    float yaw;
    float pitch;

    float fov;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), 
        glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f), 
        float yaw = -90.0f, 
        float pitch = 0.0f) 
        : front(glm::vec3(0.0f, 0.0f, -1.0f)), fov(45.0f), position(position), world_up(world_up), yaw(yaw), pitch(pitch) { update_camera_vector(); }

    glm::mat4 get_view_matrix() {
        return glm::lookAt(position, position + front, up);
    }

    void update_camera_vector() {
        glm::vec3 new_front;
        new_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        new_front.y = sin(glm::radians(pitch));
        new_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        
        front = glm::normalize(new_front);

        right = glm::normalize(glm::cross(front, world_up));
        up = glm::normalize(glm::cross(right, front));
    }
};

class Player {
public:
    glm::vec3 position;
    float yaw;
    float pitch;
    Camera camera;

    Player() : position(0.0f, 0.0f, 5.0f), yaw(-90.0f), pitch(0.0f) {
        camera.position = position + glm::vec3(0.0f, 1.8f, 0.0f);
        sync_camera();
    }

    void move(glm::vec3 direction, float deltaTime) {
        float speed = 2.5f;
        position += direction * speed * deltaTime;
        camera.position = position + glm::vec3(0.0f, 1.8f, 0.0f);
    }

    void rotate(float yawOffset, float pitchOffset) {
        yaw   += yawOffset;
        pitch += pitchOffset;

        if (pitch > 89.0f)  pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        sync_camera();
    }

private:
    void sync_camera() {
        camera.yaw = yaw;
        camera.pitch = pitch;
        camera.update_camera_vector(); 
    }
};

int window_width = WIDTH;
int window_height = HEIGHT;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    window_width = width;
    window_height = height;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

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

    Player player;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) player.move(player.camera.front, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) player.move(-player.camera.front, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) player.move(-player.camera.right, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) player.move(player.camera.right, deltaTime);

        float rotationSpeed = 100.0f;

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) player.rotate(-rotationSpeed * deltaTime, 0.0f);
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) player.rotate(rotationSpeed * deltaTime, 0.0f);

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) player.rotate(0.0f, rotationSpeed * deltaTime);
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) player.rotate(0.0f, -rotationSpeed * deltaTime);

        std::cout << "Player Pos: " 
          << player.position.x << ", " 
          << player.position.y << ", " 
          << player.position.z << std::endl;

        shader.use();

        glm::mat4 view = player.camera.get_view_matrix();

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