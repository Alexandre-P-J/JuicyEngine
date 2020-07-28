#pragma once
#include <glm/glm.hpp>
#include <Serialization.h>

struct TransformComponent {
    glm::mat4 transform = glm::mat4(1);
    glm::vec3 get_position() const {
        return glm::vec3(transform[3]);
    }

    template<class Archive>
    void serialize(Archive & ar) {
        ar(cereal::make_nvp("Transform", transform));
    }

};


