#pragma once
#include <Systems/System.h>
#include <Systems/RenderSystem.h>
#include <entt/entt.hpp>
#include <filesystem>
#include <ResourceManager.h>

namespace JuicyEngine {

class SceneManager {
    friend class Engine;
    entt::registry current_scene;
    std::vector<std::unique_ptr<System>> systems;

    SceneManager(Resource<nlohmann::json> scene);
    void load_scene(std::filesystem::path scene_path);
    void save_scene(std::filesystem::path save_path);

public:
    void update();
};


class view {
    entt::runtime_view ecs_query;
};


}  // namespace JuicyEngine
