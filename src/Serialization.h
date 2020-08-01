#pragma once
#include <Components/Component.h>
#include <Components/SerializationComponent.h>
#include <spdlog/spdlog.h>
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "entt/entity/fwd.hpp"

namespace glm {

template <class Archive>
void serialize(Archive& archive, glm::vec2& v) {
    archive(v.x, v.y);
}
template <class Archive>
void serialize(Archive& archive, glm::vec3& v) {
    archive(v.x, v.y, v.z);
}
template <class Archive>
void serialize(Archive& archive, glm::vec4& v) {
    archive(v.x, v.y, v.z, v.w);
}
template <class Archive>
void serialize(Archive& archive, glm::ivec2& v) {
    archive(v.x, v.y);
}
template <class Archive>
void serialize(Archive& archive, glm::ivec3& v) {
    archive(v.x, v.y, v.z);
}
template <class Archive>
void serialize(Archive& archive, glm::ivec4& v) {
    archive(v.x, v.y, v.z, v.w);
}
template <class Archive>
void serialize(Archive& archive, glm::uvec2& v) {
    archive(v.x, v.y);
}
template <class Archive>
void serialize(Archive& archive, glm::uvec3& v) {
    archive(v.x, v.y, v.z);
}
template <class Archive>
void serialize(Archive& archive, glm::uvec4& v) {
    archive(v.x, v.y, v.z, v.w);
}
template <class Archive>
void serialize(Archive& archive, glm::dvec2& v) {
    archive(v.x, v.y);
}
template <class Archive>
void serialize(Archive& archive, glm::dvec3& v) {
    archive(v.x, v.y, v.z);
}
template <class Archive>
void serialize(Archive& archive, glm::dvec4& v) {
    archive(v.x, v.y, v.z, v.w);
}

// glm matrices serialization
template <class Archive>
void serialize(Archive& archive, glm::mat2& m) {
    archive(m[0], m[1]);
}
template <class Archive>
void serialize(Archive& archive, glm::dmat2& m) {
    archive(m[0], m[1]);
}
template <class Archive>
void serialize(Archive& archive, glm::mat3& m) {
    archive(m[0], m[1], m[2]);
}
template <class Archive>
void serialize(Archive& archive, glm::mat4& m) {
    archive(cereal::make_nvp(
        "columns",
        std::vector{std::vector{m[0][0], m[1][0], m[2][0], m[3][0]},
                    std::vector{m[0][1], m[1][1], m[2][1], m[3][1]},
                    std::vector{m[0][2], m[1][2], m[2][2], m[3][2]},
                    std::vector{m[0][3], m[1][3], m[2][3], m[3][3]}}));
}
template <class Archive>
void serialize(Archive& archive, glm::dmat4& m) {
    archive(m[0], m[1], m[2], m[3]);
}

// template <class Archive>
// void serialize(Archive& archive, glm::quat& q) {
/// archive(q.x, q.y, q.z, q.w);
//}

template <class Archive>
void serialize(Archive& archive, glm::dquat& q) {
    archive(q.x, q.y, q.z, q.w);
}

}  // namespace glm

namespace entt {
struct serializable_entity {
    entt::entity const& entity;
    entt::registry const& registry;
};

template <class Archive>
void serialize(Archive& archive, entt::registry& r) {
    auto view = r.view<SerializationComponent>();
    for (auto& entity : view) {
        auto& name = view.get<SerializationComponent>(entity).entity_name;
        archive(cereal::make_nvp(name, serializable_entity{entity, r}));
    }
}

template <class Archive>
void serialize(Archive& archive, serializable_entity& e) {
    e.registry.visit(e.entity, [&archive, &e](const auto component) {
            spdlog::info(ComponentFactory::get_from_id(component, e.registry, e.entity)->get_name());
    });
}

}  // namespace entt
