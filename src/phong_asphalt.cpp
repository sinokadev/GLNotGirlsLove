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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader.h"
#include "mesh.h"
#include "object.h"
#include "camera.hpp"

#define WIDTH 640
#define HEIGHT 480
#define TITLE "CheeseBlock"

int window_width = WIDTH;
int window_height = HEIGHT;

void process_input(GLFWwindow *window, MovingCamera &camera, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.move(camera.front, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.move(-camera.front, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.move(-camera.right, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.move(camera.right, deltaTime);

    float rotation_speed = 500.0f;

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

unsigned int load_texture(char const * path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;

    stbi_set_flip_vertically_on_load(true);
    
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    
    if (data) {
        GLenum format;
        if (nrComponents == 1) format = GL_RED;
        else if (nrComponents == 3) format = GL_RGB;
        else if (nrComponents == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

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

    Shader shader("src/shaders/phong_asphalt.vert", "src/shaders/phong_asphalt.frag");

    std::vector<Vertex> vertices = {
        // Front Face
        {{ 0.2f,  0.2f,  0.2f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.2f, -0.2f,  0.2f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{-0.2f, -0.2f,  0.2f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{-0.2f,  0.2f,  0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},

        // Back Face
        {{ 0.2f,  0.2f, -0.2f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}},
        {{ 0.2f, -0.2f, -0.2f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.2f, -0.2f, -0.2f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.2f,  0.2f, -0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}},

        // Top Face
        {{ 0.2f,  0.2f, -0.2f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.2f,  0.2f,  0.2f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{-0.2f,  0.2f,  0.2f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{-0.2f,  0.2f, -0.2f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},

        // Bottom Face
        {{ 0.2f, -0.2f, -0.2f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{ 0.2f, -0.2f,  0.2f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.2f, -0.2f,  0.2f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.2f, -0.2f, -0.2f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},

        // Right Face
        {{ 0.2f,  0.2f, -0.2f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{ 0.2f, -0.2f, -0.2f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{ 0.2f, -0.2f,  0.2f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{ 0.2f,  0.2f,  0.2f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},

        // Left Face
        {{-0.2f,  0.2f, -0.2f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.2f, -0.2f, -0.2f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.2f, -0.2f,  0.2f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.2f,  0.2f,  0.2f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
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

    MovingCamera camera({0,0,0});

    unsigned int diffuse_map = load_texture("assets/asphalt/asphalt_BaseColor.png");
    unsigned int specular_map = load_texture("assets/asphalt/asphalt_Specular.png");
    unsigned int normal_map = load_texture("assets/asphalt/asphalt_Normal.png");
    unsigned int roughness_map = load_texture("assets/asphalt/asphalt_Roughness.png");

    float delta_time = 0.0f;
    float last_frame = 0.0f;
    
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    glfwSwapInterval(1); // V-Sync

    while (!glfwWindowShouldClose(window)) {
        // DeltaTime
        float currentFrame = static_cast<float>(glfwGetTime());
        delta_time = currentFrame - last_frame;
        last_frame = currentFrame;

        int now_time = glfwGetTime();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        process_input(window, camera, delta_time);

        // Draw
        shader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse_map);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular_map);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, normal_map);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, roughness_map);

        //camera.look_at_target({0,0,0});

        glm::mat4 view = camera.get_view_matrix();

        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)window_width / (float)window_height, 0.1f, 1000.0f);

        shader.set("view", view);
        shader.set("projection", projection);

        // Light
        shader.set("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        shader.set("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        shader.set("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
        shader.set("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

        shader.set("pointLights[0].position", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.set("pointLights[0].ambient", glm::vec3(0.0f));
        shader.set("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        shader.set("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.set("pointLights[0].constant", 1.0f);
        shader.set("pointLights[0].linear", 0.09f);
        shader.set("pointLights[0].quadratic", 0.032f);


        shader.set("material.diffuseMap", 0);
        shader.set("material.specularMap", 1);
        shader.set("material.normalMap", 2);
        shader.set("material.roughnessMap", 3);

        shader.set("material.useDiffuseMap", true);
        shader.set("material.useSpecularMap", true);
        shader.set("material.useRoughnessMap", true);
        shader.set("material.useNormalMap", true);
        
        shader.set("material.baseDiffuse", glm::vec3(0.1f,0.35f,0.1f));
        shader.set("material.baseSpecular", glm::vec3(0.45f,0.55f,0.45f));
        shader.set("material.baseRoughness", glm::vec3(1-0.25));

        shader.set("viewPos", camera.position);

        // Object
        object.scale = glm::vec3(1);
        object.position = glm::vec3(0.0f, -1.8f, .0f);

        object.draw();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    shader.destroy();
    mesh.cleanup();

    glfwTerminate();
    return 0;
}