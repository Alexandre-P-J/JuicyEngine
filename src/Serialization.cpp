#include <Serialization.h>

#include <Components/Component.h>
#include <Components/LensComponent.h>
#include <Components/SerializationComponent.h>
#include <Components/TransformComponent.h>
#include <spdlog/spdlog.h>

// GLM

void glm::to_json(nlohmann::json& json, glm::mat4 const& m) {
    json["columns"] = {{m[0][0], m[1][0], m[2][0], m[3][0]},
                       {m[0][1], m[1][1], m[2][1], m[3][1]},
                       {m[0][2], m[1][2], m[2][2], m[3][2]},
                       {m[0][3], m[1][3], m[2][3], m[3][3]}};
}

void glm::from_json(nlohmann::json const& json, glm::mat4& m) {
    // json.get_to({{m[0][0], m[1][0], m[2][0], m[3][0]},
    //{m[0][1], m[1][1], m[2][1], m[3][1]},
    //{m[0][2], m[1][2], m[2][2], m[3][2]},
    //{m[0][3], m[1][3], m[2][3], m[3][3]}});
}

// ECS

void entt::to_json(nlohmann::json& j, const serializable_entity& e) {
    e.registry.visit(e.entity, [&j, &e](const auto component) {
        ComponentFactory::save(j, component, e.registry, e.entity);
    });
}

void entt::to_json(nlohmann::json& json, entt::registry& registry) {
    auto view = registry.view<SerializationComponent>();
    for (auto& entity : view) {
        json.push_back(serializable_entity{entity, registry});
    }
}

void entt::from_json(const nlohmann::json& json, entt::registry& registry) {
    for (auto entity_obj : json) {
        auto entity = registry.create();
        for (auto component : entity_obj.items()) {
            auto& component_type = component.key();
            auto& component_data = component.value();
            ComponentFactory::load(component_data, component_type, registry,
                                   entity);
        }
    }
}

