#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <memory>

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
#define TITLE "Cheese"

class Base {
public:
    std::string title;
    int window_height;
    int window_width;

    float delta_time = 0.0f;

    GLFWwindow* window;

    Base(std::string title, int window_width, int window_height, std::string vertex_path, std::string fragment_path)
    : title(title), window_height(window_height), window_width(window_width), vertex_path(vertex_path), fragment_path(fragment_path) {}

    bool init() {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
        if (!window) {
            std::cerr << "Failed to create window" << std::endl;
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(window);

        // Callbacks
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int w, int h) {
            auto self = static_cast<Base*>(glfwGetWindowUserPointer(win));
            self->framebuffer_size_callback(win, w, h);
        });

        if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            return false;
        }

        int fb_width, fb_height;
        glfwGetFramebufferSize(window, &fb_width, &fb_height);
        glViewport(0, 0, fb_width, fb_height);

        shader.load(vertex_path, fragment_path);
            
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_DEPTH_TEST);

        return true;
    }

    void setup() {
        std::vector<Vertex> vertices = {
            {{ 1.0f,  1.0f,  0.0f}, {0.625f, 0.500f}, { 0.0000f,  1.0000f,  0.0000f}},
            {{ 1.0f, -1.0f,  0.0f}, {0.375f, 0.750f}, { 0.4472f, -0.0000f,  0.8944f}},
            {{-1.0f,  1.0f, -1.0f}, {0.875f, 0.500f}, { 0.0000f,  1.0000f,  0.0000f}},
            {{-1.0f, -1.0f, -1.0f}, {0.375f, 0.250f}, {-0.0000f, -1.0000f,  0.0000f}},
            {{-1.0f,  1.0f,  1.0f}, {0.875f, 0.750f}, { 0.0000f,  1.0000f,  0.0000f}},
            {{-1.0f, -1.0f,  1.0f}, {0.375f, 1.000f}, { 0.4472f, -0.0000f,  0.8944f}},
        };

        std::vector<unsigned int> indices = {
            0, 2, 4,
            1, 0, 4,   1, 4, 5,
            5, 4, 2,   5, 2, 3,
            3, 1, 5,
            3, 2, 0,   3, 0, 1
        };

        mesh = std::make_unique<Mesh>(vertices, indices);
        
        object = std::make_unique<Object>(*mesh, shader, glm::vec3(0), glm::vec3(0,0,0), glm::vec3(1));
        camera = std::make_unique<Camera>(glm::vec3(2, 1, 2));
    }

    void draw() {
        if (object && camera) {
            // DeltaTime
            float current_frame = static_cast<float>(glfwGetTime());
            delta_time = current_frame - last_frame;
            last_frame = current_frame;

            int now_time = glfwGetTime();

            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Draw Plane
            shader.use();

            camera->look_at_target({0,0,0});

            glm::mat4 view = camera->get_view_matrix();

            glm::mat4 projection = glm::perspective(glm::radians(camera->fov), (float)window_width / (float)window_height, 0.1f, 100.0f);

            shader.set("view", view);
            shader.set("projection", projection);

            object->rotation.y += 50.0f * delta_time;
            if (object->rotation.y > 360.0f) object->rotation.y -= 360.0f;

            object->scale = glm::vec3(0.3f,0.2f,0.3f);

            object->draw();

        }
    }

    void cleanup() {
        shader.destroy();
        mesh->cleanup();
    }

private:
    void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);

        window_width = width;
        window_height = height;
    }

    float last_frame = 0.0f;

    Shader shader;

    std::string vertex_path, fragment_path;

    std::unique_ptr<Mesh> mesh;
    std::unique_ptr<Object> object;
    std::unique_ptr<Camera> camera;
};

int main() {
    Base base(TITLE, WIDTH, HEIGHT, "src/shaders/cheese_block.vert", "src/shaders/cheese_block.frag");

    if (!base.init()) {
        return -1;
    }

    base.setup();

    while (!glfwWindowShouldClose(base.window)) {

        base.draw();

        glfwSwapBuffers(base.window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}