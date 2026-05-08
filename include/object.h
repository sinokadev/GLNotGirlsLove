// AUTO GENERATED HEADER
#ifndef OBJECT_H
#define OBJECT_H

#include "shader.h"
#include "mesh.h"

#include <vector>

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Object {
public:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(0.1f);Object(Mesh &mesh, Shader &shader, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);void draw();unsigned int get_shader_id();

private:
    Mesh &mesh;
    Shader &shader;
    unsigned int model_loc;
};

#endif // OBJECT_H
