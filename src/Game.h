#pragma once
#include <memory>
#include <entt/entt.hpp>

class Game {
    friend class Engine;

    entt::registry current_scene;
    
    entt::registry& get_scene();
    void set_scene(entt::registry& scene);
    
    void update();
};
