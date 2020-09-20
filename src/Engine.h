#pragma once
#include <InputManager.h>
#include <RenderManager.h>
#include <ResourceManager.h>
#include <SceneManager.h>
#include <ScriptingManager.h>
#include <WindowManager.h>
#include <memory>

namespace JuicyEngine {

class Engine {
    std::shared_ptr<WindowManager> window_manager;
    std::shared_ptr<RenderManager> render_manager;
    std::shared_ptr<InputManager> input_manager;
    std::shared_ptr<ResourceManager> resource_manager;
    std::shared_ptr<ScriptingManager> scripting_manager;
    std::shared_ptr<SceneManager> scene_manager;

    bool running = false;

    Engine() = default;
    void pre_run();
    void post_run();

public:
    inline std::shared_ptr<WindowManager> get_window_manager() {
        return window_manager;
    }
    inline std::shared_ptr<InputManager> get_input_manager() {
        return input_manager;
    }
    inline std::shared_ptr<ResourceManager> get_resource_manager() {
        return resource_manager;
    }
    inline std::shared_ptr<RenderManager> get_render_manager() {
        return render_manager;
    }
    inline std::shared_ptr<SceneManager> get_scene_manager() {
        return scene_manager;
    }
    inline std::shared_ptr<ScriptingManager> get_scripting_manager() {
        return scripting_manager;
    }
    inline void exit() { running = false; }

    static Engine& instance();

    void run();
};

}  // namespace JuicyEngine
