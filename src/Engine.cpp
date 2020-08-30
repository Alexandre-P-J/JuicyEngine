#include <Engine.h>
#include <spdlog/spdlog.h>

namespace JuicyEngine {

void Engine::pre_run() {
    SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_VIDEO |
             SDL_INIT_EVENTS);
    if (auto err = SDL_GetError(); err != nullptr && err[0] != '\0') {
        spdlog::warn(err);
    }

    resource_manager = std::shared_ptr<JuicyEngine::ResourceManager>(
        new JuicyEngine::ResourceManager());
    std::filesystem::path scene_path;
    if (auto config = resource_manager->load_json("config.json")) {
        auto& json = *config;
        scene_path = json.value("startup scene", "main.json");
    }

    window_manager = std::shared_ptr<JuicyEngine::WindowManager>(
        new JuicyEngine::WindowManager());
    render_manager = std::shared_ptr<JuicyEngine::RenderManager>(
        new JuicyEngine::RenderManager());
    input_manager = std::shared_ptr<JuicyEngine::InputManager>(
        new JuicyEngine::InputManager());

    auto scene = resource_manager->load_json(scene_path);
    scene_manager = std::shared_ptr<JuicyEngine::SceneManager>(
        new JuicyEngine::SceneManager(scene));
}

void Engine::post_run() {
    scene_manager.reset();
    input_manager.reset();
    resource_manager.reset();
    render_manager.reset();
    window_manager.reset();
    SDL_Quit();
}

Engine& Engine::instance() {
    static auto inst = JuicyEngine::Engine();
    return inst;
}

void Engine::run() {
    if (running) {
        spdlog::warn("Do not call JuicyEngine::Engine::run() multiple times!");
        return;
    }
    running = true;
    pre_run();
    while (running) {
        input_manager->refresh_input();
        scene_manager->update();
    }
    post_run();
}

}  // namespace JuicyEngine
