#pragma once
#include <InputManager.h>
#include <Systems/RenderSystem.h>
#include <WindowManager.h>
#include <memory>
#include "Game.h"

namespace JuicyEngine {

class Engine {
    std::shared_ptr<JuicyEngine::WindowManager> w_manager;
    std::shared_ptr<JuicyEngine::InputManager> i_manager;
    std::vector<std::unique_ptr<System>> systems;

    std::shared_ptr<Game> game;
    bool running = false;

    Engine();

public:
    inline std::shared_ptr<JuicyEngine::WindowManager> get_window_manager() {
        return w_manager;
    }
    inline std::shared_ptr<JuicyEngine::InputManager> get_input_manager() {
        return i_manager;
    }
    inline void exit() { running = false; }
    ~Engine();
    static JuicyEngine::Engine& instance();
    void run(Game* game);
};
}  // namespace JuicyEngine
