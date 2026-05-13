// AUTO GENERATED HEADER
#ifndef MESH_H
#define MESH_H

#include "shader.h"

#include <vector>

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Vertex {
    glm::vec3 Position;
    glm::vec2 TexCoords;
    glm::vec3 Normal;
    glm::vec3 Tangent;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);void draw(const Shader &shader);void cleanup();

private:
    unsigned int VAO, VBO, EBO;void setup_mesh();
};

#endif // MESH_H
