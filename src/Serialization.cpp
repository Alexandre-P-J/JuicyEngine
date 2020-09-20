#include <Serialization.h>

#include <Components/Component.h>
#include <Components/SerializationComponent.h>
#include <Logging.h>

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

void to_json(nlohmann::json& json, glm::vec3 const& v) {
    json[0] = v[0];
    json[1] = v[1];
    json[2] = v[2];
}
void from_json(nlohmann::json const& json, glm::vec3& v) {
    v[0] = json[0];
    v[1] = json[1];
    v[2] = json[2];
}

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

namespace JuicyEngine {

void to_json(nlohmann::json& json, Registry const& registry) {
    auto v = registry.view(SerializationComponent::name);
    for (auto entity : v) {
        auto& json_entity = json.emplace_back(nlohmann::json::object());
        registry.visit(entity,
                       [&json_entity, &registry, entity](const auto component) {
                           if (component != SerializationComponent::name) {
                               ComponentFactory::save(json_entity, component,
                                                      registry, entity);
                           }
                       });
    }
}

void from_json(const nlohmann::json& json, Registry& registry) {
    for (auto entity_obj : json) {
        auto entity = registry.new_entity();
        for (auto& component : entity_obj.items()) {
            auto& component_type = component.key();
            auto& component_data = component.value();
            ComponentFactory::load(component_data, component_type, registry,
                                   entity);
        }
        registry.set_component<SerializationComponent>(
            entity, SerializationComponent::name);  // implicit
    }
}

}  // namespace JuicyEngine
