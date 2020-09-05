#include <SceneManager.h>
#include <Engine.h>
#include <Serialization.h>
#include <spdlog/spdlog.h>

namespace JuicyEngine {

SceneManager::SceneManager(Resource<nlohmann::json> scene) {
    if (scene) {
        auto json = *scene;
        if (json.contains("entities") && json.contains("systems")) {
            json["entities"].get_to(current_scene);
            systems.reserve(json["systems"].size());
            for (auto& element : json["systems"].items()) {
                std::string name{element.value()};
                // check if its a lua system (has ".lua" at the end)
                if ((name.size() > 4) &&
                    (name.substr(name.size() - 4, 4) == ".lua")) {
                    // might be a lua system
                    spdlog::info("WIP " + name);
                }
                // check if its a C++ system
                else if (auto system = SystemFactory::create(name)) {
                    // is a C++ system
                    systems.push_back(std::move(system));
                }
                else {
                    spdlog::warn(name +
                                 " is not a proper system defined in C++. If "
                                 "its a lua system please use the script path "
                                 "and use the .lua extension");
                }
            }
        }
        else {
            spdlog::warn(
                "Malformed scene, scenes must have both entities and systems "
                "sections");
        }
    }
}

void SceneManager::load_scene(std::filesystem::path scene_path) {
    auto r_manager = Engine::instance().get_resource_manager();
    if (auto json = r_manager->load_json(scene_path)) {
        current_scene.clear();
        json->get_to(current_scene);
    }
}

void SceneManager::save_scene(std::filesystem::path save_path) {
    nlohmann::json json;
    entt::to_json(json, current_scene);
    try {
        std::ofstream os(save_path);
        os << std::setw(4) << json << std::endl;
    } catch (std::exception const&) {
        spdlog::warn("Error saving scene to " + save_path.string());
    }
}

void SceneManager::update() {
    for (auto const& system : systems) {
        system->update(current_scene);
    }
}

}  // namespace JuicyEngine
