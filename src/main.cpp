#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#define WIDTH 640
#define HEIGHT 480
#define TITLE "여기에 TITLE을 써 넣으시오."

class Shader {
public:

    Shader(std::string vertex_path, std::string fragment_path) {
        std::string vertex_code;
        std::string fragment_code;
        std::ifstream vertex_file;
        std::ifstream fragment_file;

        vertex_file.open(vertex_path);

        if (!vertex_file.is_open()) {
            std::cerr << "Failed to open vertex shader file: " << vertex_path << std::endl;
            return;
        }

        fragment_file.open(fragment_path);

        if (!fragment_file.is_open()) {
            std::cerr << "Failed to open fragment shader file: " << fragment_path << std::endl;
            return;
        }

        std::stringstream vertex_stream, fragment_stream;

        vertex_stream << vertex_file.rdbuf();
        fragment_stream << fragment_file.rdbuf();
        
        vertex_code = vertex_stream.str();
        fragment_code = fragment_stream.str();
        
        vertex_file.close();
        fragment_file.close();

        const char *vertex_string = vertex_code.c_str();
        const char *fragment_string = fragment_code.c_str();

        // Compile vertex shader
        unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_string, NULL);
        glCompileShader(vertex_shader);

        int success;
        char infoLog[512];
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
            std::cerr << "Failed to compile vertex shader\n" << infoLog << std::endl;
        }

        // Compile fragment shader
        unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_string, NULL);
        glCompileShader(fragment_shader);

        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
            std::cerr << "Failed to compile fragment shader\n" << infoLog << std::endl;
        }

        // Link shader
        shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);

        glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
            std::cerr << "Failed to link shaders\n" << infoLog << std::endl;
        }

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    void use() { 
        glUseProgram(shader_program); 
    }

    unsigned int getID() const {
        return shader_program;
    }

    ~Shader() {
        glDeleteProgram(shader_program);
    }

private:
    unsigned int shader_program;
};

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

    float vertices[] = {
        0.2f,  0.2f, 0.0f,
        0.2f, -0.2f, 0.0f,
        -0.2f, -0.2f, 0.0f,
        -0.2f,  0.2f, 0.0f,

        0.8f,  0.8f, 0.0f,
        0.8f,  0.4f, 0.0f,
        0.4f,  0.4f, 0.0f,
        0.4f,  0.8f, 0.0f,

        -0.4f, -0.4f, 0.0f,
        -0.4f, -0.8f, 0.0f,
        -0.8f, -0.8f, 0.0f,
        -0.8f, -0.4f, 0.0f
    };

    unsigned int indices[] = {
        0, 1, 3,   1, 2, 3,
        4, 5, 7,   5, 6, 7,
        8, 9, 11,  9, 10, 11
    };

    // VBO, VAO, EBO
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}