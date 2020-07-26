#include "RenderSystem.h"

#include <Components/LensComponent.h>
#include <Components/RenderComponent.h>
#include <Components/TransformComponent.h>
#include <Engine.h>
#include <SDL.h>
#include <bgfx/bgfx.h>
#include <spdlog/spdlog.h>

JuicyEngine::RenderSystem::RenderSystem() {
    if (!SDL_WasInit(SDL_INIT_VIDEO)) {
        spdlog::critical(
            "SDL_VIDEO not initialized before RenderSystem construction");
    }

    bgfx::renderFrame();  // Render empty frame
    bgfx::Init init;
    init.type = bgfx::RendererType::Enum::Count;
    init.resolution.width = width;
    init.resolution.height = height;
    init.resolution.reset = BGFX_RESET_VSYNC;
    bgfx::init(init);

    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f,
                       0);
    bgfx::setViewRect(0, 0, 0, width, height);
}

JuicyEngine::RenderSystem::~RenderSystem() {
    bgfx::shutdown();
}

void JuicyEngine::RenderSystem::update(entt::registry &scene) {
    auto cameras = scene.view<LensComponent, TransformComponent>();
    // WIP, for now just select an arbitrary camera
    if (auto camera = cameras.front(); scene.valid(camera)) {
        auto const &trans_c = cameras.get<TransformComponent>(camera);
        auto const &lens_c = cameras.get<LensComponent>(camera);

        bgfx::setViewTransform(0, &trans_c.transform[0][0],
                               &lens_c.projection[0][0]);
    }

    auto objects = scene.view<RenderComponent, TransformComponent>();
    for (auto entity : objects) {
        auto &render_data = objects.get<RenderComponent>(entity);
        auto &transform_data = objects.get<TransformComponent>(entity);
        bgfx::setTransform(&transform_data.transform[0][0]);
        bgfx::setVertexBuffer(0, render_data.vertices);
        bgfx::setIndexBuffer(render_data.indexes);
        // Uniforms [WIP]
        bgfx::setState(BGFX_STATE_DEFAULT);
        bgfx::submit(0, render_data.shader);
    }
    bgfx::frame();
}

// void JuicyEngine::RenderSystem::resize_window(int width, int height) {
// if (!window) {
// spdlog::critical("Cannot resize window, window is null.");
// return;
//}
// if (width <= 0 || height <= 0) {
// spdlog::critical("Cannot resize window to negative size.");
// return;
//}
// SDL_SetWindowSize(window, width, height);
// bgfx::setViewRect(0, 0, 0, width, height);
//}

// void JuicyEngine::RenderSystem::submit_current(bgfx::ProgramHandle shader) {
// bgfx::setState(BGFX_STATE_DEFAULT);
// bgfx::submit(0, shader);
//}
