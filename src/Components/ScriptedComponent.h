#pragma once
#include <Components/Component.h>
#include <Logging.h>

namespace JuicyEngine {

struct ScriptedComponent : public ComponentRegistry<ScriptedComponent> {
    static constexpr const char* name = "ScriptedComponent";
    std::string component_name;    

    ScriptedComponent() = default;
    ScriptedComponent(sol::table data, std::string const& name = "");

    sol::table data;

    void save(nlohmann::json& json) const override;
    void load(nlohmann::json const& json) override;
};

}  // namespace JuicyEngine
