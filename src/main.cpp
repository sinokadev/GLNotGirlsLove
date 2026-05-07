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
        char log[512];
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex_shader, 512, NULL, log);
            std::cerr << "Failed to compile vertex shader\n" << log << std::endl;
        }

        // Compile fragment shader
        unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_string, NULL);
        glCompileShader(fragment_shader);

        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment_shader, 512, NULL, log);
            std::cerr << "Failed to compile fragment shader\n" << log << std::endl;
        }

        // Link shader
        shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);

        glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader_program, 512, NULL, log);
            std::cerr << "Failed to link shaders\n" << log << std::endl;
        }

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    void use() { 
        glUseProgram(shader_program); 
    }

    void set(const std::string &name, bool value) const {
        glUniform1i(glGetUniformLocation(shader_program, name.c_str()), (int)value);
    }

    void set(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(shader_program, name.c_str()), value);
    }

    void set(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(shader_program, name.c_str()), value);
    }

    void set(const std::string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(shader_program, name.c_str()), 1, &value[0]);
    }

    void set(const std::string &name, const glm::mat4 &value) const {
        glUniformMatrix4fv(glGetUniformLocation(shader_program, name.c_str()), 1, GL_FALSE, &value[0][0]);
    }

    void set(int location, bool value) const {
        glUniform1i(location, value);
    }

    void set(int location, int value) const {
        glUniform1i(location, value);
    }

    void set(int location, float value) const {
        glUniform1f(location, value);
    }

    void set(int location, const glm::vec3 &value) const {
        glUniform3fv(location, 1, &value[0]);
    }

    void set(int location, const glm::mat4 &value) const {
        glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
    }

    unsigned int get_id() const {
        return shader_program;
    }

    ~Shader() {
        glDeleteProgram(shader_program);
    }

private:
    unsigned int shader_program;
};

struct Vertex {
    glm::vec3 Position;
    glm::vec2 TexCoords;
    glm::vec3 Normal;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
        this->vertices = vertices;
        this->indices = indices;

        setupMesh();
    }

    void draw(const Shader &shader) {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    ~Mesh() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

private:
    unsigned int VAO, VBO, EBO;

    void setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        glBindVertexArray(0);
    }
};

class Object {
public:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(0.1f);

    Object(Mesh &mesh, Shader &shader, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) : 
    mesh(mesh),
    shader(shader),
    position(position),
    rotation(rotation),
    scale(scale) {
        model_loc = glGetUniformLocation(shader.get_id(), "model");
    }

    void draw() {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        model = glm::scale(model, scale);

        shader.set(model_loc, model);

        mesh.draw(shader);
    }

    unsigned int get_shader_id() {
        return shader.get_id();
    }

private:
    Mesh &mesh;
    Shader &shader;
    unsigned int model_loc;
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

    std::vector<Vertex> vertices = {
        // 첫 번째 사각형 (중앙 근처)
        {{ 0.2f,  0.2f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}, // 0
        {{ 0.2f, -0.2f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}, // 1
        {{-0.2f, -0.2f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}, // 2
        {{-0.2f,  0.2f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}, // 3
    };

    std::vector<unsigned int> indices = {
        0, 1, 3,   1, 2, 3,    // 첫 번째 사각형
    };

    Mesh mesh(vertices, indices);
    Object object(mesh, shader, glm::vec3(0), glm::vec3(0,0,1), glm::vec3(1));

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        object.draw();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}