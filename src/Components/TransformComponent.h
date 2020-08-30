#pragma once
#include <Components/Component.h>
#include <Serialization.h>
#include <glm/glm.hpp>

struct TransformComponent : public ComponentRegistry<TransformComponent> {
    glm::mat4 transform = glm::mat4(1);
    glm::vec3 get_position() const { return glm::vec3(transform[3]); }

    static constexpr const char* name = "Transform";
    
    void save(nlohmann::json& json) const override { json = transform; }

    void load(nlohmann::json const& json) override { json.get_to(transform); }
};

