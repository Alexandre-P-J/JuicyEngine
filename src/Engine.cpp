#include <Engine.h>
#include <spdlog/spdlog.h>

#include <Components/LensComponent.h>
#include <Components/RenderComponent.h>
#include <Components/TransformComponent.h>
#include <Systems/RenderSystem.h>
#include <VertexLayouts.h>
#include <bx/math.h>
#include <glm/glm.hpp>
#include "ResourceManager.h"

#include <Components/Component.h>
#include <Components/LensComponent.h>
#include <Components/RenderComponent.h>
#include <Components/SerializationComponent.h>
#include <Components/TransformComponent.h>
#include <Serialization.h>
#include <entt/entity/helper.hpp>
#include <entt/entity/registry.hpp>
#include <entt/entity/snapshot.hpp>
#include <fstream>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <sstream>
#include <vector>
namespace JuicyEngine {

void Engine::pre_run() {
    SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_VIDEO |
             SDL_INIT_EVENTS);
    if (auto err = SDL_GetError(); err != nullptr && err[0] != '\0') {
        spdlog::warn(err);
    }
    window_manager = std::shared_ptr<JuicyEngine::WindowManager>(
        new JuicyEngine::WindowManager());
    render_manager = std::shared_ptr<JuicyEngine::RenderManager>(
        new JuicyEngine::RenderManager());
    resource_manager = std::shared_ptr<JuicyEngine::ResourceManager>(
        new JuicyEngine::ResourceManager());
    input_manager = std::shared_ptr<JuicyEngine::InputManager>(
        new JuicyEngine::InputManager());
    scene_manager = std::shared_ptr<JuicyEngine::SceneManager>(
        new JuicyEngine::SceneManager());
    systems = SystemFactory::create_all();
}

void Engine::post_run() {
    systems.clear();
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
    {
        if (running) {
            spdlog::warn(
                "Do not call JuicyEngine::Engine::run() multiple times!");
            return;
        }
        running = true;
        pre_run();

        // std::ifstream is("save.json");
        // nlohmann::json json;
        // is >> json;
        auto registry = entt::registry();
        // json.get_to(registry);

        // TEST START
        auto camera = registry.create();
        registry.emplace<TransformComponent>(camera);
        registry.emplace<LensComponent>(camera);
        registry.emplace<SerializationComponent>(camera);

        auto obj = registry.create();
        auto& t = registry.emplace<TransformComponent>(obj);
        t.transform = glm::scale(glm::mat4(1), glm::vec3(10, 10, 10));

        // PosVertex vertices[] = {{25.0f, 25.0f, 0.0f},
        //{25.0f, -25.0f, 0.0f},
        //{-25.0f, -25.0f, 0.0f},
        //{-25.0f, 25.0f, 0.0f}};
        // auto verticesh =
        // bgfx::createVertexBuffer(bgfx::makeRef(vertices, sizeof(vertices)),
        // PosVertex::ms_layout);

        // const uint32_t indices[] = {0, 3, 2, 2, 1, 0};
        // auto indicesh =
        // bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)),
        // BGFX_BUFFER_INDEX32);
        auto r = resource_manager->default_sprite;  // load_mesh("plane.obj");
        auto indicesh = std::get<1>(*r);
        auto verticesh = std::get<0>(*r);
        bgfx::ProgramHandle shaderh;
        auto sp = resource_manager->load_program("basic/v_simple.bin",
                                                 "basic/f_simple.bin");
        if (sp) {
            auto r = RenderComponent();
            r.vertices = verticesh;
            r.indexes = indicesh;
            r.shader = sp;
            registry.emplace<RenderComponent>(obj, r);
        }
        registry.emplace<SerializationComponent>(obj);
        
        scene_manager->set_scene(registry);

        // TEST END
        while (running) {
            input_manager->refresh_input();
            scene_manager->update(systems);
        }
    }
    // nlohmann::json json;
    // entt::to_json(json, registry);
    // std::ofstream os("save.json");
    // os << std::setw(4) << json << std::endl;

    post_run();
}

}  // namespace JuicyEngine
