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

#include "shader.h"
#include "mesh.h"
#include "object.h"

#define WIDTH 640
#define HEIGHT 480
#define TITLE "여기에 TITLE을 써 넣으시오." 

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
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

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); 

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

        shader.set("view", view);
        shader.set("projection", projection);

        object.draw();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}