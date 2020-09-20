#pragma once
#include <Systems/System.h>
#include <ResourceManager.h>
#include <sol/sol.hpp>

namespace JuicyEngine {

class ScriptedSystem : public SystemRegistry<ScriptedSystem> {
    sol::protected_function update_func;
    std::string system_name;
public:
    static constexpr const char* name = "ScriptedSystem";
    ScriptedSystem() = default;
    ScriptedSystem(sol::protected_function const func, std::string const& name = "");
    void update(Registry& scene) override;
};

}  // namespace JuicyEngine
