#pragma once
#include <Systems/System.h>
#include <Systems/RenderSystem.h>
#include <Utils/ECS.h>
#include <filesystem>
#include <ResourceManager.h>

namespace JuicyEngine {

class SceneManager {
    friend class Engine;
    Registry current_scene;
    std::vector<std::unique_ptr<System>> systems;

    SceneManager(Resource<nlohmann::json> scene);
    void load_scene(std::filesystem::path scene_path);
    void save_scene(std::filesystem::path save_path);

public:
    void update();
};

}  // namespace JuicyEngine
