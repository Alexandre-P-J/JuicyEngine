#include <SceneManager.h>

#include <fstream>
#include <nlohmann/json.hpp>
#include <Serialization.h>

namespace JuicyEngine {

    void SceneManager::set_scene(std::filesystem::path path) {
        std::ifstream is(path);
        nlohmann::json json;
        is >> json;
        auto registry = entt::registry();
        json.get_to(registry);
        current_scene = std::move(registry);
    }


    void SceneManager::update(std::vector<std::unique_ptr<System>> const& systems) {
        for (auto const& system : systems) {
            system->update(current_scene);
        } 
    }

}
