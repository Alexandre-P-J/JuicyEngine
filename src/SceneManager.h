#pragma once
#include <Systems/System.h>
#include <entt/entt.hpp>
#include <filesystem>

namespace JuicyEngine {

class SceneManager {
    friend class Engine;
    entt::registry current_scene;
    SceneManager() = default;

public:
    void set_scene(std::filesystem::path path);
    inline void set_scene(entt::registry& r) {
        current_scene = std::move(r);
    }
    void update(std::vector<std::unique_ptr<System>> const& systems);
};

}  // namespace JuicyEngine
