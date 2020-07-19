#pragma once
#include <glm/glm.hpp>

struct TransformComponent {
    glm::mat4 transform = glm::mat4(1);
    glm::vec3 get_position() const {
        return glm::vec3(transform[3]);
    }
};
