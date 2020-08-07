#pragma once
#include <Components/Component.h>
#include <Serialization.h>
#include <glm/glm.hpp>

struct TransformComponent : public ComponentRegistry<TransformComponent> {
    static constexpr const char* name = "Transform";
    glm::mat4 transform = glm::mat4(1);
    glm::vec3 get_position() const { return glm::vec3(transform[3]); }

    template <class Archive>
    void save(Archive& ar) const {
        ar(cereal::make_nvp("Transform", transform));
    }
    template <class Archive>
    void load(Archive& ar);
};

