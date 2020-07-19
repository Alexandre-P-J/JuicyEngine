#include <Engine.h>
#include <spdlog/spdlog.h>

#include <Components/LensComponent.h>
#include <Components/TransformComponent.h>
#include <Components/RenderComponent.h>
#include "ResourceManager.h"
#include <VertexLayouts.h>
#include <bx/math.h>
#include <glm/glm.hpp>
#include <Systems/System.h>

JuicyEngine::Engine::Engine() {
    SDL_Init(0);
}




JuicyEngine::Engine::~Engine() {
    SDL_Quit();
}



std::shared_ptr<JuicyEngine::Engine> JuicyEngine::Engine::get_instance() {
    static std::shared_ptr<JuicyEngine::Engine> ptr(new Engine);
    return ptr;
}




void JuicyEngine::Engine::run(Game* game_ptr) {
    auto test = JuicyEngine::System::create("RenderSystem");
    if (running) {
        spdlog::warn("Do not call JuicyEngine::Engine::run() multiple times!");
        return;
    }
    running = true;
    game = std::shared_ptr<Game>(game_ptr);
    
    //TEST START
    
    auto&& registry = game->get_scene();
    auto camera = registry.create();
    registry.emplace<TransformComponent>(camera);
    registry.emplace<LensComponent>(camera);
    
    auto obj = registry.create();
    //glm::mat4 test;
    //bx::Vec3 eye = {0.f, 0.f, -1.f};
    //bx::Vec3 at = {0.f, 0.f, 1.f};
    //bx::mtxLookAt(&test[0][0], eye, at);
    registry.emplace<TransformComponent>(obj);
    
    
    PosColorVertex vertices[] = {{25.0f, 25.0f, 5.0f, 0xff0000ff},
                                        {25.0f, -25.0f, 0.0f, 0xff00ffff},
                                        {-25.0f, -25.0f, 0.0f, 0xffff0000},
                                        {-25.0f, 25.0f, 0.0f, 0xff00ff00}};
    auto verticesh = bgfx::createVertexBuffer(
        bgfx::makeRef(vertices, sizeof(vertices)), PosColorVertex::ms_layout);

    const uint16_t indices[] = {0, 3, 2, 2, 1, 0};
    auto indicesh =
        bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)));
    bgfx::ProgramHandle shaderh;
    auto sp = ResourceManager::get_instance()->load_shader_program(
        "basic/v_simple.bin", "basic/f_simple.bin");
    if (sp) {
        shaderh = sp.value();
    } else
        spdlog::critical("Default sprite shader couldn't be loaded!");
    
    
    registry.emplace<RenderComponent>(obj, verticesh, indicesh, shaderh);  
    
    //TEST END


    while(running) {
        game->update();
        renderer.update(game->get_scene());
    }
}
