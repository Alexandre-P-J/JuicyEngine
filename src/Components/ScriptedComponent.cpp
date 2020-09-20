#include <Components/ScriptedComponent.h>

namespace JuicyEngine {

ScriptedComponent::ScriptedComponent(sol::table init, std::string const& name)
    : data(init), component_name(name) {}

void ScriptedComponent::save(nlohmann::json& json) const {
    if (data.valid()) {
        auto save = data["save"];
        if (save.valid()) {
            sol::protected_function_result ret = save(data, std::ref(json));
            if (!ret.valid()) {
                Logging::Engine(
                    Level::warning,
                    (component_name.empty() ? "" : component_name + " ") +
                        "Component aborted while saving to json");
            }
        }
        else {
            Logging::Engine(
                Level::warning,
                (component_name.empty() ? "" : component_name + " ") +
                    "Component save variable is not a valid function");
        }
    }
}
void ScriptedComponent::load(nlohmann::json const& json) {
    if (data.valid()) {
        sol::object o = data["load"];
        if (o.valid() && o.get_type() == sol::type::function) {
            sol::protected_function load = data["load"];
            sol::protected_function_result ret = load(data, std::ref(json));
            if (!ret.valid()) {
                Logging::Engine(
                    Level::warning,
                    (component_name.empty() ? "" : component_name + " ") +
                        "Component aborted while loading from json");
            }
        }
        else {
            Logging::Engine(
                Level::warning,
                (component_name.empty() ? "" : component_name + " ") +
                    "Component load variable is not a valid function");
        }
    }
}
}  // namespace JuicyEngine
