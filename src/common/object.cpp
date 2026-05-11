#include "shader.h"
#include "mesh.h"
#include "object.h"

#include <vector>

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Object::Object(Mesh &mesh, Shader &shader, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) : 
mesh(mesh),
shader(shader),
position(position),
rotation(rotation),
scale(scale) {
    model_loc = glGetUniformLocation(shader.get_id(), "model");
}

void Object::draw() {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, scale);

    shader.set(model_loc, model);

    mesh.draw(shader);
}

unsigned int Object::get_shader_id() {
    return shader.get_id();
}