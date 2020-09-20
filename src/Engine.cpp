#include <Engine.h>
#include <spdlog/spdlog.h>

namespace JuicyEngine {

void Engine::pre_run() {
    SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_VIDEO |
             SDL_INIT_EVENTS);
    if (auto err = SDL_GetError(); err != nullptr && err[0] != '\0') {
        Logging::Engine(Level::warning, err);
    }

    resource_manager = std::shared_ptr<ResourceManager>(new ResourceManager());
    window_manager = std::shared_ptr<WindowManager>(new WindowManager());
    render_manager = std::shared_ptr<RenderManager>(new RenderManager());
    input_manager = std::shared_ptr<InputManager>(new InputManager());
    scripting_manager =
        std::shared_ptr<ScriptingManager>(new ScriptingManager());
    scene_manager = std::shared_ptr<SceneManager>(new SceneManager());

    if (auto config = resource_manager->load_json("config.json")) {
        auto& json = *config;
        std::filesystem::path scene_path =
            json.value("startup scene", "main.json");
        scene_manager->load_scene(scene_path);
    }
}

void Engine::post_run() {
    scene_manager->save_scene("testsave.json");
    scene_manager.reset();
    scripting_manager.reset();
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
        Logging::Engine(
            Level::warning,
            "Do not call JuicyEngine::Engine::run() multiple times!");
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
