#include "RenderSystem.h"

#include <Components/LensComponent.h>
#include <Components/RenderComponent.h>
#include <Components/TransformComponent.h>
#include <Engine.h>
#include <bgfx/bgfx.h>

namespace JuicyEngine {

void JuicyEngine::RenderSystem::update(Registry &scene) {
    auto cameras = scene.view(LensComponent::name, TransformComponent::name);
    // WIP, for now just select an arbitrary camera
    if (auto it = cameras.begin(); it != cameras.end()) {
        auto camera = *it;
        auto const trans_c =
            scene.get<TransformComponent>(camera, TransformComponent::name);
        auto const lens_c =
            scene.get<LensComponent>(camera, LensComponent::name);

        bgfx::setViewTransform(0, &(trans_c->transform[0][0]),
                               &(lens_c->projection[0][0]));
    }

    auto objects = scene.view(RenderComponent::name, TransformComponent::name);
    for (auto entity : objects) {
        auto render_data =
            scene.get<RenderComponent>(entity, RenderComponent::name);
        auto transform_data =
            scene.get<TransformComponent>(entity, TransformComponent::name);
        bgfx::setTransform(&(transform_data->transform[0][0]));
        if (auto mesh = render_data->mesh) {
            bgfx::setVertexBuffer(0, mesh->first);
            bgfx::setIndexBuffer(mesh->second);

            // Uniforms [WIP]
            bgfx::setState(BGFX_STATE_DEFAULT);
            if (auto shader = render_data->shader) bgfx::submit(0, *shader);
        }
    }
    bgfx::frame();
}

}  // namespace JuicyEngine

// void JuicyEngine::RenderSystem::resize_window(int width, int height) {
// if (!window) {
// Logging::Engine(Level::critical, "Cannot resize window, window is null.");
// return;
//}
// if (width <= 0 || height <= 0) {
// Logging::Engine(Level::critical, "Cannot resize window to negative size.");
// return;
//}
// SDL_SetWindowSize(window, width, height);
// bgfx::setViewRect(0, 0, 0, width, height);
//}

// void JuicyEngine::RenderSystem::submit_current(bgfx::ProgramHandle shader) {
// bgfx::setState(BGFX_STATE_DEFAULT);
// bgfx::submit(0, shader);
//}
