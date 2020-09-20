#include <Systems/ScriptedSystem.h>

namespace JuicyEngine {

ScriptedSystem::ScriptedSystem(sol::protected_function const func,
                               std::string const& name)
    : update_func(func), system_name(name) {}

void ScriptedSystem::update(Registry& scene) {
    if (update_func) {
        sol::protected_function_result ret = update_func(scene);
        if (!ret.valid()) {
            update_func = sol::nil;
            Logging::Engine(Level::warning,
                            (system_name.empty() ? "" : system_name + " ") +
                                "System aborted execution");
        }
    }
}

}  // namespace JuicyEngine
