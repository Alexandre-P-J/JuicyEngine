#pragma once
#include <Components/Component.h>
#include <Serialization.h>
#include <bx/math.h>
#include <glm/glm.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/json.hpp>

struct LensComponent : public ComponentRegistry<LensComponent> {
    static constexpr const char* name = "Lens";
    glm::mat4 projection;

    LensComponent() { set_projection(0.f, 500.f, 0.f, 500.f, 128); }

    void set_projection(float left, float right, float bottom, float top,
                        float view_distance) {
        bx::mtxOrtho(&projection[0][0], left, right, bottom, top, 0.f,
                     view_distance, 0.f, true);
    }

    template <class Archive>
    void save(Archive& ar) const {
        ar(cereal::make_nvp("Projection", projection));
    }
    template <class Archive>
    void load(Archive& ar);
};

//CEREAL_REGISTER_TYPE(LensComponent);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, LensComponent);
