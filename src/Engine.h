#pragma once
#include <InputManager.h>
#include <RenderManager.h>
#include <ResourceManager.h>
#include <SceneManager.h>
#include <Systems/System.h>
#include <WindowManager.h>
#include <memory>
#include "Game.h"

namespace JuicyEngine {

class Engine {
    std::shared_ptr<JuicyEngine::WindowManager> window_manager;
    std::shared_ptr<JuicyEngine::RenderManager> render_manager;
    std::shared_ptr<JuicyEngine::InputManager> input_manager;
    std::shared_ptr<JuicyEngine::ResourceManager> resource_manager;
    std::shared_ptr<JuicyEngine::SceneManager> scene_manager;
    std::vector<std::unique_ptr<System>> systems;

    bool running = false;

    Engine() = default;
    void pre_run();
    void post_run();

public:
    inline std::shared_ptr<JuicyEngine::WindowManager> get_window_manager() {
        return window_manager;
    }
    inline std::shared_ptr<JuicyEngine::InputManager> get_input_manager() {
        return input_manager;
    }
    inline std::shared_ptr<JuicyEngine::ResourceManager>
    get_resource_manager() {
        return resource_manager;
    }
    inline std::shared_ptr<JuicyEngine::RenderManager> get_render_manager() {
        return render_manager;
    }
    inline std::shared_ptr<JuicyEngine::SceneManager> get_scene_manager() {
        return scene_manager;
    }
    inline void exit() { running = false; }

    static JuicyEngine::Engine& instance();

    void run();
};

}  // namespace JuicyEngine
