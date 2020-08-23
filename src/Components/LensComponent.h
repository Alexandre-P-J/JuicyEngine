#pragma once
#include <Components/Component.h>
#include <bx/math.h>
#include <glm/glm.hpp>
#include <Serialization.h>
struct LensComponent : public ComponentRegistry<LensComponent> {
    static constexpr const char* name = "Lens";
    glm::mat4 projection;

    LensComponent() { set_projection(0.f, 500.f, 0.f, 500.f, 128); }

    void set_projection(float left, float right, float bottom, float top,
                        float view_distance) {
        bx::mtxOrtho(&projection[0][0], left, right, bottom, top, 0.f,
                     view_distance, 0.f, true);
    }

    // template <class Archive>
    // void save(Archive& ar) const {
    // ar(cereal::make_nvp("Projection", projection));
    //}
    // template <class Archive>
    // void load(Archive& ar);
void save(nlohmann::json& json) const override { 
    //json = projection; 
}
void load(nlohmann::json const &json) override {
    //json.get_to(projection);
}
};

