#pragma once
#include <Systems/System.h>
#include <Systems/RenderSystem.h>
#include <Utils/ECS.h>
#include <filesystem>
#include <ResourceManager.h>
#include <sol/sol.hpp>
#include <string>

namespace JuicyEngine {

class SceneManager {
    friend class Engine;
    Registry current_scene;
    
    std::vector<std::unique_ptr<System>> systems;

    // Serialization
    std::vector<std::string> loaded_script_names;
    std::vector<std::string> loaded_system_names;

    void load_scene(std::filesystem::path const& scene_path);
    void save_scene(std::filesystem::path const& save_path);

public:
    SceneManager();
    Registry& get_scene();
    void update();
};

}  // namespace JuicyEngine
