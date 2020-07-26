#pragma once
#include <memory>
#include <entt/entt.hpp>

class Game {
    entt::registry current_scene;
    
    public:
    entt::registry& get_scene();
    void set_scene(entt::registry& scene);
    
    void update();
};
