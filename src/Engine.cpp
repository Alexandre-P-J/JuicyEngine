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

#include <sstream>
#include <vector>
#undef Bool // X11, ...
#include <cereal/archives/json.hpp>
#include <entt/entity/registry.hpp>
#include <entt/entity/snapshot.hpp>
#include <entt/entity/helper.hpp>
#include <fstream>
#include <Components/Component.h>
#include <Components/LensComponent.h>
#include <Components/RenderComponent.h>
#include <Components/TransformComponent.h>
#include <Components/SerializationComponent.h>
#include <Serialization.h>

JuicyEngine::Engine::Engine() {
    SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_VIDEO |
             SDL_INIT_EVENTS);
    if (auto err = SDL_GetError(); err != nullptr && err[0] != '\0') {
        spdlog::warn(err);
    }
    w_manager = std::shared_ptr<JuicyEngine::WindowManager>(
        new JuicyEngine::WindowManager());
    i_manager = std::shared_ptr<JuicyEngine::InputManager>(
        new JuicyEngine::InputManager());
}

JuicyEngine::Engine::~Engine() {
    systems.clear();
    w_manager.reset();
    i_manager.reset();
    SDL_Quit();
}

JuicyEngine::Engine& JuicyEngine::Engine::instance() {
    static auto inst = JuicyEngine::Engine();
    return inst;
}

void JuicyEngine::Engine::run(Game* game_ptr) {
    if (running) {
        spdlog::warn("Do not call JuicyEngine::Engine::run() multiple times!");
        return;
    }
    running = true;
    // systems should be initialized outside constructor just in case they use
    // the instance function
    systems = SystemFactory::create_all();
    game = std::shared_ptr<Game>(game_ptr);

    std::ifstream is("cereal.json");
    {
        cereal::JSONInputArchive input {is};
        entt::load(input, game->get_scene());
    } 


    // TEST START

    //auto&& registry = game->get_scene();
    //auto camera = registry.create();
    //registry.emplace<TransformComponent>(camera);
    //registry.emplace<LensComponent>(camera);

    //auto obj = registry.create();
    //registry.emplace<TransformComponent>(obj);

    //PosColorVertex vertices[] = {{25.0f, 25.0f, 5.0f, 0xff0000ff},
                                 //{25.0f, -25.0f, 0.0f, 0xff00ffff},
                                 //{-25.0f, -25.0f, 0.0f, 0xffff0000},
                                 //{-25.0f, 25.0f, 0.0f, 0xff00ff00}};
    //auto verticesh = bgfx::createVertexBuffer(
        //bgfx::makeRef(vertices, sizeof(vertices)), PosColorVertex::ms_layout);

    //const uint16_t indices[] = {0, 3, 2, 2, 1, 0};
    //auto indicesh =
        //bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)));
    //bgfx::ProgramHandle shaderh;
    //auto sp = ResourceManager::get_instance()->load_shader_program(
        //"basic/v_simple.bin", "basic/f_simple.bin");
    //if (sp) {
        //shaderh = sp.value();
    //}
    //else
        //spdlog::critical("Default sprite shader couldn't be loaded!");

    //auto r = RenderComponent();
    //r.vertices = verticesh;
    //r.indexes = indicesh;
    //r.shader = shaderh;
    //registry.emplace<RenderComponent>(obj, r);
    //registry.emplace<SerializationComponent>(obj);

    // TEST END
    while (running) {
        i_manager->refresh_input();
        game->update();
        for (auto& system : systems) {
            system->update(game->get_scene());
        }
    }

    //std::ofstream os("cereal.json");
    //{
        //cereal::JSONOutputArchive output{os};
        //entt::save(output, registry);
    //}
}
