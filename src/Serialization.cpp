#include <Serialization.h>

#include <Components/Component.h>
#include <Components/SerializationComponent.h>
#include <spdlog/spdlog.h>

namespace std {

void to_json(nlohmann::json& json, filesystem::path const& m) {
    json = m.string();
}
void from_json(nlohmann::json const& json, filesystem::path& m) {
    string str;
    json.get_to(str);
    m = filesystem::path(str);
}

}  // namespace std

namespace glm {

void to_json(nlohmann::json& json, glm::mat4 const& m) {
    json["columns"] = {{m[0][0], m[1][0], m[2][0], m[3][0]},
                       {m[0][1], m[1][1], m[2][1], m[3][1]},
                       {m[0][2], m[1][2], m[2][2], m[3][2]},
                       {m[0][3], m[1][3], m[2][3], m[3][3]}};
}

void from_json(nlohmann::json const& json, glm::mat4& m) {
    if (json.contains("rows")) {
        auto& arr = json["rows"];
        for (auto i = 0; i < 4; ++i)
            for (auto j = 0; j < 4; ++j) m[i][j] = arr[i][j];
    }
    else if (json.contains("columns")) {
        auto& arr = json["columns"];
        for (auto i = 0; i < 4; ++i)
            for (auto j = 0; j < 4; ++j) m[i][j] = arr[j][i];
    }
    else
        m = glm::mat4(1);  // identity matrix
}

}  // namespace glm

namespace entt {

void to_json(nlohmann::json& j, const serializable_entity& e) {
    e.registry.visit(e.entity, [&j, &e](const auto component) {
        if (component != entt::type_info<SerializationComponent>::id()) {
            ComponentFactory::save(j, component, e.registry, e.entity);
        }
    });
}

void to_json(nlohmann::json& json, entt::registry& registry) {
    auto view = registry.view<SerializationComponent>();
    for (auto& entity : view) {
        json.push_back(serializable_entity{entity, registry});
    }
}

void from_json(const nlohmann::json& json, entt::registry& registry) {
    for (auto entity_obj : json) {
        auto entity = registry.create();
        for (auto component : entity_obj.items()) {
            auto& component_type = component.key();
            auto& component_data = component.value();
            ComponentFactory::load(component_data, component_type, registry,
                                   entity);
        }
        registry.emplace_or_replace<SerializationComponent>(
            entity);  // implicit
    }
}

}  // namespace entt
