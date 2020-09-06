#pragma once
#include <Components/Component.h>
#include <Serialization.h>
#include <bx/math.h>
#include <glm/glm.hpp>

namespace JuicyEngine {

struct LensComponent : public ComponentRegistry<LensComponent> {
    static constexpr const char* name = "Lens";
    glm::mat4 projection;

    LensComponent() { set_projection(0.f, 500.f, 0.f, 500.f, 128); }

    void set_projection(float left, float right, float bottom, float top,
                        float view_distance) {
        bx::mtxOrtho(&projection[0][0], left, right, bottom, top, 0.f,
                     view_distance, 0.f, true);
    }

    void save(nlohmann::json& json) const override { json = projection; }
    void load(nlohmann::json const& json) override { json.get_to(projection); }
};

}  // namespace JuicyEngine
