#include <Components/ScriptedComponent.h>
#include <Engine.h>
#include <Logging.h>
#include <SceneManager.h>
#include <Serialization.h>
#include <Systems/ScriptedSystem.h>

namespace JuicyEngine {

SceneManager::SceneManager() {}

void SceneManager::load_scene(std::filesystem::path const& scene_path) {
    auto r_manager = Engine::instance().get_resource_manager();
    auto scene = r_manager->load_json(scene_path);
    // validate resource
    if (!scene) {
        return;
    }

    // validate the 3 scene sections exist
    auto const& json = *scene;
    if (!(json.contains("systems") && json.contains("scripts") &&
          json.contains("entities"))) {
        Logging::Engine(
            Level::warning,
            "Malformed scene: systems, scripts and entities sections are "
            "required.");
        return;
    }
    // scripts
    auto scripting_manager = Engine::instance().get_scripting_manager();
    loaded_script_names.reserve(json["scripts"].size());
    for (auto const& script : json["scripts"].items()) {
        std::filesystem::path script_path{script.value()};
        loaded_script_names.push_back(script_path);
        if (auto data = r_manager->load_text(script_path)) {
            // hopefully systems / component registertering
            scripting_manager->run_script(*data, script_path);
        }
    }
    // systems
    loaded_system_names.reserve(json["systems"].size());
    systems.reserve(json["systems"].size());
    for (auto const& system : json["systems"].items()) {
        std::string name{system.value()};
        loaded_system_names.push_back(name);
        if (auto sys = SystemFactory::create(name)) {
            systems.push_back(std::move(sys));
        }
        else {
            Logging::Engine(Level::warning,
                            name + " does not name a registered system");
        }
    }
    // entities
    json["entities"].get_to(current_scene);
}

void SceneManager::save_scene(std::filesystem::path const& save_path) {
    nlohmann::json json;
    json["systems"] = loaded_system_names;
    json["scripts"] = loaded_script_names;
    to_json(json["entities"], current_scene);
    try {
        std::ofstream os(save_path);
        os << std::setw(4) << json << std::endl;
    } catch (std::exception const&) {
        Logging::Engine(Level::warning,
                        "Error saving scene to " + save_path.string());
    }
}

void SceneManager::update() {
    for (auto const& system : systems) {
        system->update(current_scene);
    }
}

Registry& SceneManager::get_scene() { return current_scene; }

}  // namespace JuicyEngine
